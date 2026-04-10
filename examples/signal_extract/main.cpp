/*
 *  dbc-parser signal extraction example.
 *  Loads a DBC file, then decodes hardcoded CAN frames to demonstrate
 *  signal extraction with physical value conversion.
 *
 *  NOTE: This is example code demonstrating the decode pipeline, not a
 *  library API. The signal extraction logic here handles simple multiplexing
 *  but does not resolve cascaded extended multiplexing (SG_MUL_VAL_).
 *  A production implementation should be based on this pattern.
 *
 *  Build:
 *    cd examples/signal_extract
 *    cmake -B build
 *    cmake --build build
 *
 *  Run:
 *    ./build/dbc_signal path/to/file.dbc
 */

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

#include "dbc/dbcfile.h"
#include "dbc/extract.h"

using namespace std;

// ---------------------------------------------------------------------------
// Signal extraction from raw CAN bytes
// Ported from proven SignalExtractor (sw_dlconfigurator)
// ---------------------------------------------------------------------------

// Intel (little-endian): startBit is LSB position, bits numbered sequentially.
static uint64_t extractRawIntel(const uint8_t* data, uint32_t dlc,
                                uint32_t startBit, uint32_t bitLen) {
    uint64_t result = 0;
    int bytepos = startBit / 8;
    int bitoffset = startBit % 8;
    int help = bitoffset + static_cast<int>(bitLen);
    int cpyLen = help / 8;
    if (help % 8) cpyLen++;

    for (int i = 0; i < cpyLen && (bytepos + i) < static_cast<int>(dlc); i++)
        result |= static_cast<uint64_t>(data[bytepos + i]) << (8 * i);

    result >>= bitoffset;
    if (bitLen < 64)
        result &= (1ULL << bitLen) - 1;
    return result;
}

// Convert Vector Motorola start bit to Intel-equivalent start bit.
static int vectorMotorolaStartBitToIntel(int startBit, int bitLen) {
    int pos = 7 - (startBit % 8) + (bitLen - 1);
    if (pos < 8)
        return startBit - bitLen + 1;
    else
        return (startBit / 8) * 8 + (7 - (startBit % 8));
}

// Motorola (big-endian): startBit is MSB position in Vector bit numbering.
// Uses the proven byte-reverse-copy approach from SignalExtractor.
static uint64_t extractRawMotorola(const uint8_t* data, uint32_t dlc,
                                   uint32_t startBit, uint32_t bitLen) {
    static const int bitsToConsume[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    uint32_t realStartBit = static_cast<uint32_t>(
        vectorMotorolaStartBitToIntel(static_cast<int>(startBit), static_cast<int>(bitLen)));

    int help = static_cast<int>(bitLen) - bitsToConsume[startBit % 8];
    if (help < 0) help = 0;
    int cpyLen = 1 + (help / 8);
    if (help % 8) cpyLen++;
    int bytepos = static_cast<int>(startBit / 8) + (cpyLen - 1);

    uint64_t result = 0;
    for (int i = 0; i < cpyLen && bytepos >= 0 && bytepos < static_cast<int>(dlc); i++)
        result |= static_cast<uint64_t>(data[bytepos--]) << (8 * i);

    int bitoffset = 8 - static_cast<int>((realStartBit + bitLen) % 8);
    if (bitoffset == 8) bitoffset = 0;

    result >>= bitoffset;
    if (bitLen < 64)
        result &= (1ULL << bitLen) - 1;
    return result;
}

static int64_t signExtend(uint64_t val, uint32_t bitLen) {
    if (bitLen == 0 || bitLen >= 64) return static_cast<int64_t>(val);
    if (val & (1ULL << (bitLen - 1)))
        val |= ~((1ULL << bitLen) - 1);
    return static_cast<int64_t>(val);
}

struct DecodedSignal {
    string name;
    uint64_t raw = 0;
    double physical = 0.0;
    string unit;
    string value_desc;
};

static DecodedSignal decodeSignal(const dbc::Signal& sig,
                                  const uint8_t* data, uint32_t dlc) {
    DecodedSignal dec;
    dec.name = sig.name();
    dec.unit = sig.unit();

    // Extract raw bits
    if (sig.byte_order() == dbc::BYTE_ORDER_LITTLE_ENDIAN)
        dec.raw = extractRawIntel(data, dlc, sig.start_bit(), sig.bit_length());
    else
        dec.raw = extractRawMotorola(data, dlc, sig.start_bit(), sig.bit_length());

    // Apply sign extension and conversion
    if (sig.is_signed()) {
        int64_t signedRaw = signExtend(dec.raw, sig.bit_length());
        dec.physical = sig.offset() + sig.factor() * static_cast<double>(signedRaw);
    } else {
        dec.physical = sig.offset() + sig.factor() * static_cast<double>(dec.raw);
    }

    // Match value description (use signed raw for comparison)
    int64_t cmpVal = sig.is_signed()
        ? signExtend(dec.raw, sig.bit_length())
        : static_cast<int64_t>(dec.raw);
    for (const auto& vd : sig.value_descriptions()) {
        if (vd.value() == cmpVal) {
            dec.value_desc = vd.description();
            break;
        }
    }

    return dec;
}

// ---------------------------------------------------------------------------
// Decode a CAN frame against a DBC message
// ---------------------------------------------------------------------------

static void decodeFrame(const dbc::DbcFile& dbc, uint32_t canId,
                        const uint8_t* data, uint32_t dlc) {
    const dbc::Message* msg = nullptr;
    for (int i = 0; i < dbc.messages_size(); i++) {
        if (dbc.messages(i).id() == canId) {
            msg = &dbc.messages(i);
            break;
        }
    }

    if (!msg) {
        cout << "  ID 0x" << hex << canId << dec << ": no matching message" << endl;
        return;
    }

    cout << "  " << msg->name() << " (0x" << hex << canId << dec
         << ", DLC=" << dlc << "):" << endl;

    // Pass 1: find multiplexer value (if any)
    int64_t muxValue = -1;
    for (const auto& sig : msg->signals()) {
        if (sig.multiplex_type() == dbc::MULTIPLEX_MULTIPLEXOR ||
            sig.multiplex_type() == dbc::MULTIPLEX_MULTIPLEXED_AND_MULTIPLEXOR) {
            DecodedSignal mux = decodeSignal(sig, data, dlc);
            muxValue = static_cast<int64_t>(mux.raw);
            break;
        }
    }

    // Pass 2: decode all active signals
    for (const auto& sig : msg->signals()) {
        // Skip multiplexed signals that don't match current mux value
        if (muxValue >= 0) {
            if (sig.multiplex_type() == dbc::MULTIPLEX_MULTIPLEXED) {
                if (sig.multiplex_value() != static_cast<uint32_t>(muxValue))
                    continue;
            }
            // m<N>M signals: check their mux value against parent before decoding
            if (sig.multiplex_type() == dbc::MULTIPLEX_MULTIPLEXED_AND_MULTIPLEXOR) {
                if (sig.multiplex_value() != static_cast<uint32_t>(muxValue))
                    continue;
            }
        }

        DecodedSignal dec = decodeSignal(sig, data, dlc);
        cout << "    " << left << setw(30) << dec.name
             << " raw=" << setw(8) << dec.raw
             << " -> " << fixed << setprecision(2) << setw(12) << dec.physical;
        if (!dec.unit.empty()) cout << " " << dec.unit;
        if (!dec.value_desc.empty()) cout << "  [" << dec.value_desc << "]";
        cout << endl;
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file.dbc>" << endl;
        cerr << endl;
        cerr << "Decodes fabricated CAN frames using DBC signal definitions." << endl;
        cerr << "Edit the demo frames in main() for your own CAN data." << endl;
        return 1;
    }

    // 1. Parse and extract
    auto file = dbcfile::Loader::readDbcFile(argv[1]);
    if (!file) {
        cerr << "Failed to load: " << argv[1] << endl;
        return 1;
    }
    dbc::DbcFile dbc = dbc::extract::extractFile(file.get());

    cout << "Loaded: " << argv[1] << " (" << dbc.messages_size() << " messages)" << endl;
    cout << endl;

    // 2. Decode demo frames (fabricated bytes for first 3 messages)
    cout << "--- Decoding demo frames ---" << endl;

    for (int i = 0; i < dbc.messages_size() && i < 3; i++) {
        const auto& msg = dbc.messages(i);
        uint8_t data[8];
        for (int b = 0; b < 8; b++)
            data[b] = static_cast<uint8_t>((i + 1) * 0x11 + b * 0x10);

        decodeFrame(dbc, msg.id(), data, msg.dlc() > 0 ? msg.dlc() : 8);
        cout << endl;
    }

    // 3. Custom frame example
    cout << "--- Custom frame example ---" << endl;
    if (dbc.messages_size() > 0) {
        const auto& firstMsg = dbc.messages(0);
        uint8_t custom[8] = {0x00, 0x64, 0x00, 0x32, 0xFF, 0xFF, 0xFF, 0xFF};
        cout << "  Raw bytes: ";
        for (int b = 0; b < 8; b++)
            cout << hex << setw(2) << setfill('0') << (int)custom[b] << " ";
        cout << dec << setfill(' ') << endl;
        decodeFrame(dbc, firstMsg.id(), custom, 8);
    }

    return 0;
}

/*
 *  dbc-parser signal decode example.
 *  Loads a DBC file, then decodes fabricated CAN frames through the supported
 *  dbc::decode API (bit extraction, sign extension, physical conversion,
 *  SIG_VALTYPE_ floats, and full simple + extended multiplexer resolution).
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
#include <iomanip>
#include <iostream>
#include <vector>

#include "dbc/dbcfile.h"
#include "dbc/extract.h"
#include "dbc/decode.h"

using namespace std;

static void decodeAndPrint(const dbc::DbcFile& db, const dbc::Message& msg,
                           const vector<uint8_t>& payload) {
    cout << "  " << msg.name() << " (0x" << hex << msg.id() << dec
         << ", " << payload.size() << " bytes):" << endl;

    for (const auto& d : dbc::decode::decodeFrame(msg, payload.data(), payload.size())) {
        cout << "    " << left << setw(30) << d.signal->name()
             << " raw=" << setw(10) << d.raw_signed
             << " -> " << fixed << setprecision(3) << setw(14) << d.physical;
        if (!d.signal->unit().empty()) cout << " " << d.signal->unit();
        if (d.value_description) cout << "  [" << *d.value_description << "]";
        cout << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file.dbc>" << endl;
        cerr << "Decodes fabricated CAN frames using DBC signal definitions." << endl;
        return 1;
    }

    auto file = dbcfile::Loader::readDbcFile(argv[1]);
    if (!file) {
        cerr << "Failed to load: " << argv[1] << endl;
        return 1;
    }
    dbc::DbcFile db = dbc::extract::extractFile(file.get());

    cout << "Loaded: " << argv[1] << " (" << db.messages_size() << " messages)"
         << endl << endl;

    cout << "--- Decoding demo frames ---" << endl;
    for (int i = 0; i < db.messages_size() && i < 3; i++) {
        const auto& msg = db.messages(i);
        size_t bytes = msg.dlc() > 0 ? msg.dlc() : 8;
        if (bytes > 64) bytes = 64;    // CAN-FD caps payload at 64 bytes
        vector<uint8_t> payload(bytes);
        for (size_t b = 0; b < bytes; b++)
            payload[b] = static_cast<uint8_t>((i + 1) * 0x11 + b * 0x10);
        decodeAndPrint(db, msg, payload);
        cout << endl;
    }

    cout << "--- Custom frame example ---" << endl;
    if (const dbc::Message* msg = dbc::decode::findMessage(
            db, db.messages_size() > 0 ? db.messages(0).id() : 0,
            db.messages_size() > 0 ? db.messages(0).is_extended_id() : false)) {
        vector<uint8_t> custom = {0x00, 0x64, 0x00, 0x32, 0xFF, 0xFF, 0xFF, 0xFF};
        cout << "  Raw bytes:";
        for (uint8_t b : custom)
            cout << " " << hex << setw(2) << setfill('0') << (int)b;
        cout << dec << setfill(' ') << endl;
        decodeAndPrint(db, *msg, custom);
    }

    return 0;
}

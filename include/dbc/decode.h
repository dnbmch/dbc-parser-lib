#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "dbc/dbc.pb.h"

namespace dbc::decode {

// One decoded signal value. `signal` and `value_description` borrow from the
// caller's proto objects; keep the DbcFile alive while results are in use.
struct DecodedSignal {
    const dbc::Signal* signal = nullptr;
    uint64_t raw        = 0;    // extracted bits, zero-extended
    int64_t  raw_signed = 0;    // sign-extended when is_signed, else == raw
    double   physical   = 0.0;  // offset + factor * value
    const std::string* value_description = nullptr;  // matched VAL_ entry, or null
};

// Extract the raw bit field of `sig` from a payload. Returns nullopt when the
// field does not fit inside payload_bytes (frame shorter than the layout needs).
std::optional<uint64_t> extractRaw(const dbc::Signal& sig,
                                   const uint8_t* payload, size_t payload_bytes);

// Sign-extend a bit_length-wide value (bit_length 1..64).
int64_t signExtend(uint64_t raw, uint32_t bit_length);

// Decode one signal. Multiplexing is NOT consulted — the caller asserts this
// signal is present in the frame. Returns nullopt when the field does not fit.
std::optional<DecodedSignal> decodeSignal(const dbc::Signal& sig,
                                          const uint8_t* payload,
                                          size_t payload_bytes);

// Decode every signal of `msg` that is active in this frame: multiplexing
// (simple + extended, cascades resolved) filters inactive signals; signals
// that do not fit the actual payload are omitted. Output order follows
// msg.signals() order.
std::vector<DecodedSignal> decodeFrame(const dbc::Message& msg,
                                       const uint8_t* payload,
                                       size_t payload_bytes);

// Find a message by canonical bus identity — the (id, extended) pair, id with
// bit 31 already stripped, same identity extraction uses internally.
const dbc::Message* findMessage(const dbc::DbcFile& file,
                                uint32_t id, bool is_extended);

} // namespace dbc::decode

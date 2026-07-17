#pragma once

#include "dbc/dbcfile.h"
#include "dbc/dbc.pb.h"
#include "dbc/common.pb.h"

namespace dbc::extract {

// Top-level entry point: parse tree -> typed proto contract.
dbc::DbcFile extractFile(dbcfile::DbcFile* file);

// UTF-8 sanitizer for protobuf string fields. Replaces ill-formed byte
// sequences with U+FFFD so every output string is guaranteed-valid UTF-8.
std::string sanitizeUtf8(const std::string& s);

// A CAN ID decoded into its canonical value and extended-frame flag.
struct CanId {
    uint32_t id = 0;
    bool is_extended = false;
};

// Decode a raw DBC ID (bit 31 = extended) into (canonical id, extended flag).
CanId canonicalizeId(uint32_t rawId);

} // namespace dbc::extract

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

// Message lookup helper - finds message by raw DBC ID (handles extended bit).
dbc::Message* findMessageByRawId(dbc::DbcFile& result, uint32_t rawId);
// Signal lookup within a message.
dbc::Signal* findSignalInMessage(dbc::Message* msg, const std::string& name);

// Enum converters.
dbc::ByteOrder byteOrderFromRaw(uint8_t raw);
dbc::MultiplexType muxTypeFromStr(const std::string& indicator);
dbc::ValueType valueTypeFromRaw(uint32_t raw);
dbc::AttributeValueType attrTypeFromStr(const std::string& type);
dbc::AttributeScope attrScopeFromStr(const std::string& scope);
dbc::EnvironmentVariableType envVarTypeFromRaw(uint32_t raw);
dbc::RelationalAttributeScope relAttrScopeFromStr(const std::string& scope);

} // namespace dbc::extract

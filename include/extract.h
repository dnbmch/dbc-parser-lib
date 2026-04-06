#pragma once

#include "dbcfile.h"
#include "dbc.pb.h"
#include "common.pb.h"

namespace extract {

// UTF-8 sanitizer for protobuf string fields
std::string sanitizeUtf8(const std::string& s);

// Message lookup helper - finds message by raw DBC ID (handles extended bit)
dbc::v1::Message* findMessageByRawId(dbc::v1::DbcFile& result, uint32_t rawId);
// Signal lookup within a message
dbc::v1::Signal* findSignalInMessage(dbc::v1::Message* msg, const std::string& name);

// Top-level entry point
dbc::v1::DbcFile extractFile(dbcfile::DbcFile* file);

// Per-concept extraction (internal)
dbc::v1::Message extractMessage(const dbcfile::RawMessage& raw);
dbc::v1::Signal extractSignal(const dbcfile::RawSignal& raw);
dbc::v1::AttributeDefinition extractAttributeDef(const dbcfile::RawAttributeDef& raw);
dbc::v1::AttributeDefault extractAttributeDefault(const dbcfile::RawAttributeDefault& raw);
dbc::v1::AttributeValue extractAttributeValue(const dbcfile::RawAttributeValue& raw);
dbc::v1::ValueTable extractValueTable(const dbcfile::RawValueTable& raw);

// Enum converters
dbc::v1::ByteOrder byteOrderFromRaw(uint8_t raw);
dbc::v1::MultiplexType muxTypeFromStr(const std::string& indicator);
dbc::v1::ValueType valueTypeFromRaw(uint32_t raw);
dbc::v1::AttributeValueType attrTypeFromStr(const std::string& type);
dbc::v1::AttributeScope attrScopeFromStr(const std::string& scope);

// Phase 2 extraction
dbc::v1::EnvironmentVariable extractEnvVar(const dbcfile::RawEnvironmentVariable& raw);
dbc::v1::SignalGroup extractSignalGroup(const dbcfile::RawSignalGroup& raw);
dbc::v1::ExtendedMultiplexValue extractExtendedMux(const dbcfile::RawExtendedMux& raw);
dbc::v1::RelationalAttributeDefinition extractRelAttrDef(const dbcfile::RawRelationalAttributeDef& raw);
dbc::v1::RelationalAttributeValue extractRelAttrValue(const dbcfile::RawRelationalAttributeValue& raw);

// Enum converters (Phase 2)
dbc::v1::EnvironmentVariableType envVarTypeFromRaw(uint32_t raw);
dbc::v1::RelationalAttributeScope relAttrScopeFromStr(const std::string& scope);

// Merging (comments, values, attributes into messages/signals/nodes)
void mergeComments(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawComment>& comments);
void mergeValueDescriptions(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawValueDescription>& vds);
void mergeSigValTypes(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawSigValType>& svts);
void mergeAttributeValues(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawAttributeValue>& avs);
void mergeTxBus(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawTxBu>& txbus);
void mergeEnvVarData(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawEnvVarData>& evds);
void mergeEnvVarComments(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawComment>& comments);
void mergeExtendedMuxToSignals(dbc::v1::DbcFile& result, const std::vector<dbcfile::RawExtendedMux>& emuxs);

} // namespace extract

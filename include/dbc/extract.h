#pragma once

#include "dbc/dbcfile.h"
#include "dbc/dbc.pb.h"
#include "dbc/common.pb.h"

namespace dbc::extract {

// UTF-8 sanitizer for protobuf string fields
std::string sanitizeUtf8(const std::string& s);

// Message lookup helper - finds message by raw DBC ID (handles extended bit)
dbc::Message* findMessageByRawId(dbc::DbcFile& result, uint32_t rawId);
// Signal lookup within a message
dbc::Signal* findSignalInMessage(dbc::Message* msg, const std::string& name);

// Top-level entry point
dbc::DbcFile extractFile(dbcfile::DbcFile* file);

// Per-concept extraction (internal)
dbc::Message extractMessage(const dbcfile::RawMessage& raw);
dbc::Signal extractSignal(const dbcfile::RawSignal& raw);
dbc::AttributeDefinition extractAttributeDef(const dbcfile::RawAttributeDef& raw);
dbc::AttributeDefault extractAttributeDefault(const dbcfile::RawAttributeDefault& raw);
dbc::AttributeValue extractAttributeValue(const dbcfile::RawAttributeValue& raw);
dbc::ValueTable extractValueTable(const dbcfile::RawValueTable& raw);

// Enum converters
dbc::ByteOrder byteOrderFromRaw(uint8_t raw);
dbc::MultiplexType muxTypeFromStr(const std::string& indicator);
dbc::ValueType valueTypeFromRaw(uint32_t raw);
dbc::AttributeValueType attrTypeFromStr(const std::string& type);
dbc::AttributeScope attrScopeFromStr(const std::string& scope);

// Phase 2 extraction
dbc::EnvironmentVariable extractEnvVar(const dbcfile::RawEnvironmentVariable& raw);
dbc::SignalGroup extractSignalGroup(const dbcfile::RawSignalGroup& raw);
dbc::ExtendedMultiplexValue extractExtendedMux(const dbcfile::RawExtendedMux& raw);
dbc::RelationalAttributeDefinition extractRelAttrDef(const dbcfile::RawRelationalAttributeDef& raw);
dbc::RelationalAttributeValue extractRelAttrValue(const dbcfile::RawRelationalAttributeValue& raw);

// Enum converters (Phase 2)
dbc::EnvironmentVariableType envVarTypeFromRaw(uint32_t raw);
dbc::RelationalAttributeScope relAttrScopeFromStr(const std::string& scope);

// Merging (comments, values, attributes into messages/signals/nodes)
void mergeComments(dbc::DbcFile& result, const std::vector<dbcfile::RawComment>& comments);
void mergeValueDescriptions(dbc::DbcFile& result, const std::vector<dbcfile::RawValueDescription>& vds);
void mergeSigValTypes(dbc::DbcFile& result, const std::vector<dbcfile::RawSigValType>& svts);
void mergeAttributeValues(dbc::DbcFile& result, const std::vector<dbcfile::RawAttributeValue>& avs);
void mergeTxBus(dbc::DbcFile& result, const std::vector<dbcfile::RawTxBu>& txbus);
void mergeEnvVarData(dbc::DbcFile& result, const std::vector<dbcfile::RawEnvVarData>& evds);
void mergeEnvVarComments(dbc::DbcFile& result, const std::vector<dbcfile::RawComment>& comments);
void mergeExtendedMuxToSignals(dbc::DbcFile& result, const std::vector<dbcfile::RawExtendedMux>& emuxs);

} // namespace dbc::extract

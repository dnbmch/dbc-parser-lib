#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace dbcfile {

struct RawSignal {
    std::string name;
    std::string mux_indicator;  // "", "M", "m0", "m1", etc.
    uint32_t start_bit = 0;
    uint32_t bit_length = 0;
    uint8_t byte_order = 1;     // 0=Motorola, 1=Intel
    char sign = '+';            // '+' or '-'
    double factor = 1.0;
    double offset = 0.0;
    double min = 0.0;
    double max = 0.0;
    std::string unit;
    std::vector<std::string> receivers;
};

struct RawMessage {
    uint32_t id = 0;            // raw (bit 31 = extended)
    std::string name;
    uint32_t dlc = 0;
    std::string sender;
    std::vector<RawSignal> signals;
};

struct RawComment {
    std::string scope;          // "", "BU_", "BO_", "SG_", "EV_"
    uint32_t message_id = 0;
    std::string signal_name;
    std::string node_name;
    std::string text;
};

struct RawAttributeDef {
    std::string scope;          // "", "BU_", "BO_", "SG_"
    std::string name;
    std::string type;           // "INT", "FLOAT", "STRING", "ENUM", "HEX"
    std::string min_val;
    std::string max_val;
    std::vector<std::string> enum_values;
};

struct RawAttributeDefault {
    std::string name;
    std::string value;
};

struct RawAttributeValue {
    std::string name;
    std::string scope;          // "", "BU_", "BO_", "SG_"
    uint32_t message_id = 0;
    std::string signal_name;
    std::string node_name;
    std::string value;
};

struct RawValueDescription {
    uint32_t message_id = 0;
    std::string signal_name;
    std::vector<std::pair<int64_t, std::string>> entries;
};

struct RawValueTable {
    std::string name;
    std::vector<std::pair<int64_t, std::string>> entries;
};

struct RawSigValType {
    uint32_t message_id = 0;
    std::string signal_name;
    uint32_t value_type = 0;    // 1=float32, 2=float64
};

struct RawTxBu {
    uint32_t message_id = 0;
    std::vector<std::string> transmitters;
};

struct RawEnvironmentVariable {
    std::string name;
    uint32_t var_type = 0;      // 0=integer, 1=float, 2=string
    double min = 0.0;
    double max = 0.0;
    std::string unit;
    double initial_value = 0.0;
    uint32_t ev_id = 0;
    std::string access_type;    // first access node (DUMMY_NODE_*)
    std::vector<std::string> access_nodes; // remaining access nodes
};

struct RawEnvVarData {
    std::string name;
    uint32_t data_size = 0;
};

struct RawSignalGroup {
    uint32_t message_id = 0;
    std::string name;
    uint32_t repetitions = 0;
    std::vector<std::string> signal_names;
};

struct RawMuxRange {
    uint32_t from = 0;
    uint32_t to = 0;
};

struct RawExtendedMux {
    uint32_t message_id = 0;
    std::string signal_name;
    std::string mux_signal_name;
    std::vector<RawMuxRange> ranges;
};

struct RawRelationalAttributeDef {
    std::string rel_scope;      // "BU_SG_REL_", "BU_BO_REL_"
    std::string name;
    std::string type;           // "INT", "FLOAT", "STRING", "ENUM"
    std::string min_val;
    std::string max_val;
    std::vector<std::string> enum_values;
};

struct RawRelationalAttributeDefault {
    std::string name;
    std::string value;
};

struct RawRelationalAttributeValue {
    std::string name;
    std::string rel_scope;      // "BU_SG_REL_", "BU_BO_REL_"
    std::string node_name;
    uint32_t message_id = 0;
    std::string signal_name;    // for BU_SG_REL_ only
    std::string value;
};

struct DbcFile {
    std::string version;
    std::vector<std::string> new_symbols;
    uint32_t bus_speed = 0;
    std::vector<std::string> nodes;
    std::vector<RawMessage> messages;
    std::vector<RawComment> comments;
    std::vector<RawAttributeDef> attribute_defs;
    std::vector<RawAttributeDefault> attribute_defaults;
    std::vector<RawAttributeValue> attribute_values;
    std::vector<RawValueDescription> value_descriptions;
    std::vector<RawValueTable> value_tables;
    std::vector<RawSigValType> sig_val_types;
    std::vector<RawTxBu> tx_bus;
    std::vector<RawEnvironmentVariable> env_vars;
    std::vector<RawEnvVarData> env_var_data;
    std::vector<RawSignalGroup> signal_groups;
    std::vector<RawExtendedMux> extended_mux;
    std::vector<RawRelationalAttributeDef> rel_attribute_defs;
    std::vector<RawRelationalAttributeDefault> rel_attribute_defaults;
    std::vector<RawRelationalAttributeValue> rel_attribute_values;
};

struct Loader {
    static std::unique_ptr<DbcFile> readDbcFile(const std::string& path);
};

} // namespace dbcfile

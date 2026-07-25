#pragma once

#include "dbc/dbc.pb.h"
#include "dbc/common.pb.h"

namespace dbc::extract {

// Run the semantic validation checks over an already-extracted file and append
// their results to file.validation_findings(). This is an opt-in pass: parsing
// and extraction never call it, so extractFile() output is byte-stable without
// it. The checks are a pure function of the proto contract (message/signal
// layout, multiplex conditions, receivers, ids) and never touch the parse tree.
//
// Checks: signal-bit overlap between non-mutually-exclusive signals, signals
// that decode past the frame DLC, duplicate (id, is_extended_id) message keys,
// and receivers not declared in BU_.
void validate(dbc::DbcFile& file);

} // namespace dbc::extract

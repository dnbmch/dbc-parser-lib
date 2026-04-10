/*
 *  dbc-parser basic example.
 *  Loads a DBC file, extracts to protobuf, prints a human-readable summary.
 *
 *  Build:
 *    cd examples/basic
 *    cmake -B build
 *    cmake --build build
 *
 *  Run:
 *    ./build/dbc_basic path/to/file.dbc
 */

#include <iomanip>
#include <iostream>
#include <string>

#include "dbc/dbcfile.h"
#include "dbc/extract.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file.dbc>" << endl;
        return 1;
    }

    // 1. Parse the DBC file
    auto file = dbcfile::Loader::readDbcFile(argv[1]);
    if (!file) {
        cerr << "Failed to load: " << argv[1] << endl;
        return 1;
    }

    // 2. Extract into the protobuf contract
    dbc::DbcFile result = dbc::extract::extractFile(file.get());

    // 3. Walk the extracted data
    cout << "DBC: " << argv[1] << endl;
    cout << "Version: \"" << result.version() << "\"" << endl;

    if (result.nodes_size() > 0) {
        cout << "\nNodes (" << result.nodes_size() << "):" << endl;
        for (const auto& node : result.nodes())
            cout << "  " << node.name() << endl;
    }

    cout << "\nMessages (" << result.messages_size() << "):" << endl;
    for (const auto& msg : result.messages()) {
        cout << "  " << msg.name()
             << "  ID=0x" << hex << msg.id() << dec
             << (msg.is_extended_id() ? " EXT" : "")
             << "  DLC=" << msg.dlc()
             << "  sender=" << msg.sender()
             << endl;

        if (!msg.comment().empty())
            cout << "    // " << msg.comment() << endl;

        for (const auto& sig : msg.signals()) {
            cout << "    " << sig.name()
                 << "  [" << sig.start_bit() << "|" << sig.bit_length()
                 << "@" << (sig.byte_order() == dbc::BYTE_ORDER_LITTLE_ENDIAN ? "1" : "0")
                 << (sig.is_signed() ? "-" : "+")
                 << "]";

            // Conversion
            cout << "  (" << sig.factor() << "," << sig.offset() << ")";
            cout << "  [" << sig.min() << "|" << sig.max() << "]";
            if (!sig.unit().empty())
                cout << " \"" << sig.unit() << "\"";

            // Multiplexing
            if (sig.multiplex_type() == dbc::MULTIPLEX_MULTIPLEXOR)
                cout << " MUX";
            else if (sig.multiplex_type() == dbc::MULTIPLEX_MULTIPLEXED)
                cout << " m" << sig.multiplex_value();
            else if (sig.multiplex_type() == dbc::MULTIPLEX_MULTIPLEXED_AND_MULTIPLEXOR)
                cout << " m" << sig.multiplex_value() << "M";

            cout << endl;

            if (!sig.comment().empty())
                cout << "      // " << sig.comment() << endl;

            // Value descriptions
            for (const auto& vd : sig.value_descriptions())
                cout << "      " << vd.value() << " = \"" << vd.description() << "\"" << endl;
        }
    }

    // Environment variables
    if (result.environment_variables_size() > 0) {
        cout << "\nEnvironment variables (" << result.environment_variables_size() << "):" << endl;
        for (const auto& ev : result.environment_variables()) {
            cout << "  " << ev.name()
                 << "  type=" << dbc::EnvironmentVariableType_Name(ev.var_type())
                 << "  [" << ev.min() << "|" << ev.max() << "]";
            if (!ev.unit().empty()) cout << " \"" << ev.unit() << "\"";
            cout << endl;
        }
    }

    // Signal groups
    if (result.signal_groups_size() > 0) {
        cout << "\nSignal groups (" << result.signal_groups_size() << "):" << endl;
        for (const auto& sg : result.signal_groups()) {
            cout << "  msg=0x" << hex << sg.message_id() << dec
                 << " " << sg.name() << ":";
            for (const auto& sn : sg.signal_names()) cout << " " << sn;
            cout << endl;
        }
    }

    // 4. Serialize to binary
    string binary;
    result.SerializeToString(&binary);
    cout << "\nSerialized: " << binary.size() << " bytes" << endl;

    return 0;
}

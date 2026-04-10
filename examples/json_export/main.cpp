/*
 *  dbc-parser JSON export example.
 *  Loads a DBC file, extracts to protobuf, serializes to JSON.
 *  The JSON output can be consumed by Python, web UIs, databases, etc.
 *
 *  Build:
 *    cd examples/json_export
 *    cmake -B build
 *    cmake --build build
 *
 *  Run:
 *    ./build/dbc_json path/to/file.dbc              # JSON to stdout
 *    ./build/dbc_json path/to/file.dbc output.json   # JSON to file
 */

#include <fstream>
#include <iostream>
#include <string>

#include "dbc/dbcfile.h"
#include "dbc/extract.h"

#include <google/protobuf/util/json_util.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file.dbc> [output.json]" << endl;
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

    // 3. Configure JSON output (pretty-print with enum names)
    google::protobuf::util::JsonPrintOptions opts;
    opts.add_whitespace = true;

    // 4. Serialize to JSON
    string json;
    auto status = google::protobuf::util::MessageToJsonString(result, &json, opts);
    if (!status.ok()) {
        cerr << "JSON serialization failed: " << status.message() << endl;
        return 1;
    }

    // 5. Write to file or stdout
    if (argc >= 3) {
        ofstream out(argv[2]);
        if (!out.is_open()) {
            cerr << "Cannot open output file: " << argv[2] << endl;
            return 1;
        }
        out << json;
        out.close();
        cerr << "Wrote " << json.size() << " bytes to " << argv[2] << endl;
    } else {
        cout << json << endl;
    }

    return 0;
}

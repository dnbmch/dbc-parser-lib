# dbc-parser-lib

Prebuilt static library, public headers, and protobuf schema for the **dbc-parser** — a C++17 library that parses DBC (CAN database) files into Protocol Buffer messages.

**Author:** Danube Mechatronics Kft.

## Downloads

Prebuilt static libraries are available on the [Releases](https://github.com/dnbmch/dbc-parser-lib/releases) page:

| Artifact | Platform |
|----------|----------|
| `dbcextract-x86_64-windows-mingw` | Windows MinGW GCC (.a) |
| `dbcextract-x86_64-linux-gnu` | Linux x86_64 (.a) |
| `dbcextract-aarch64-linux-gnu` | Linux ARM64 (.a) |
| `dbcextract-headers` | Public headers and proto files |

## Quick Start

```bash
# 1. Clone this repo
git clone https://github.com/dnbmch/dbc-parser-lib.git
cd dbc-parser-lib

# 2. Download and extract the prebuilt library for your platform
#    (from the Releases page, extract into lib/)
mkdir -p lib
tar xzf dbcextract-x86_64-linux-gnu-v0.1.0.tar.gz -C lib/

# 3. Build the examples
cmake -B build -DDBC_LIB_DIR=lib
cmake --build build

# 4. Run
./build/dbc_basic path/to/file.dbc
```

## Contents

| Directory | Description |
|-----------|-------------|
| `include/` | Public C++ headers (`dbcfile.h`, `extract.h`) |
| `proto/` | Protobuf schema files (`.proto`) for multi-language binding generation |
| `examples/` | Example applications (basic summary, JSON export, signal extraction) |

## Integration

```cpp
#include "dbcfile.h"
#include "extract.h"

auto file = dbcfile::Loader::readDbcFile("path/to/file.dbc");
dbc::v1::DbcFile result = extract::extractFile(file.get());

for (const auto& msg : result.messages()) {
    // Access signals, attributes, value descriptions, etc.
}
```

## Build Requirements

- C++17 compiler (GCC, Clang, or MSVC)
- Protocol Buffers (protobuf) runtime library

## License

Dual licensed: GPL-2.0 or Commercial. See [LICENSE.md](LICENSE.md).

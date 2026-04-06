# dbc-parser-lib

Prebuilt static library, public headers, and protobuf schema for the **dbc-parser** — a C++17 library that parses DBC (CAN database) files into Protocol Buffer messages.

**Author:** Danube Mechatronics Kft.

## Contents

| Directory | Description |
|-----------|-------------|
| `include/` | Public C++ headers (`dbcfile.h`, `extract.h`) |
| `proto/` | Protobuf schema files (`.proto`) for multi-language binding generation |
| `examples/` | Example applications (basic summary, JSON export, signal extraction) |

## Quick Start

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

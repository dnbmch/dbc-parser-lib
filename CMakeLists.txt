cmake_minimum_required(VERSION 3.14)
project(dbcextract-lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Find protobuf (needed for proto generation and linking)
# ---------------------------------------------------------------------------
find_package(Protobuf CONFIG QUIET)
if(NOT TARGET protobuf::libprotobuf)
    find_package(Protobuf REQUIRED)
endif()
find_package(absl CONFIG QUIET)

# ---------------------------------------------------------------------------
# Prebuilt static library
# ---------------------------------------------------------------------------
# Set DBC_LIB_DIR to the directory containing libdbcextract.a
# e.g. cmake -B build -DDBC_LIB_DIR=path/to/dbcextract-x86_64-linux-gnu
if(NOT DBC_LIB_DIR)
    set(DBC_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib" CACHE PATH
        "Directory containing prebuilt libdbcextract.a and generated .pb.h files")
endif()
get_filename_component(DBC_LIB_DIR "${DBC_LIB_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(dbcextract STATIC IMPORTED GLOBAL)
set_target_properties(dbcextract PROPERTIES
    IMPORTED_LOCATION "${DBC_LIB_DIR}/libdbcextract.a"
)

target_include_directories(dbcextract INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DBC_LIB_DIR}
)

target_link_libraries(dbcextract INTERFACE protobuf::libprotobuf)

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------
option(DBC_BUILD_EXAMPLES "Build example applications" ON)

if(DBC_BUILD_EXAMPLES)
    add_executable(dbc_basic examples/basic/main.cpp)
    target_link_libraries(dbc_basic PRIVATE dbcextract)

    add_executable(dbc_json examples/json_export/main.cpp)
    target_link_libraries(dbc_json PRIVATE dbcextract)

    add_executable(dbc_signal examples/signal_extract/main.cpp)
    target_link_libraries(dbc_signal PRIVATE dbcextract)
endif()

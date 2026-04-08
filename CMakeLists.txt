cmake_minimum_required(VERSION 3.15)
project(dbcextract-lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0091 NEW)

# ---------------------------------------------------------------------------
# Find protobuf (needed for linking)
# ---------------------------------------------------------------------------
find_package(Protobuf CONFIG QUIET)
if(NOT TARGET protobuf::libprotobuf)
    find_package(Protobuf REQUIRED)
endif()
find_package(absl CONFIG QUIET)

# ---------------------------------------------------------------------------
# Prebuilt static library
# ---------------------------------------------------------------------------
if(NOT DBC_LIB_DIR)
    set(DBC_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib" CACHE PATH
        "Directory containing prebuilt dbcextract library and generated .pb.h files")
endif()
get_filename_component(DBC_LIB_DIR "${DBC_LIB_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Find the library file (.a on MinGW/Linux, .lib on MSVC)
find_library(DBC_LIB_PATH NAMES dbcextract PATHS "${DBC_LIB_DIR}" NO_DEFAULT_PATH)
if(NOT DBC_LIB_PATH)
    # Fallback: try explicit names
    if(EXISTS "${DBC_LIB_DIR}/libdbcextract.a")
        set(DBC_LIB_PATH "${DBC_LIB_DIR}/libdbcextract.a")
    elseif(EXISTS "${DBC_LIB_DIR}/dbcextract.lib")
        set(DBC_LIB_PATH "${DBC_LIB_DIR}/dbcextract.lib")
    else()
        message(FATAL_ERROR "Could not find dbcextract library in ${DBC_LIB_DIR}")
    endif()
endif()

add_library(dbcextract STATIC IMPORTED GLOBAL)
set_target_properties(dbcextract PROPERTIES
    IMPORTED_LOCATION "${DBC_LIB_PATH}"
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

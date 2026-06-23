cmake_minimum_required(VERSION 3.14)
project(dbc_basic_example LANGUAGES CXX)

# Pull in dbc-parser as a subdirectory unless the parent build already defines
# the target (this CMakeLists is built both standalone and from the repo root).
# Adjust the path if you place this example elsewhere.
if(NOT TARGET dbcparser)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/dbcparser)
endif()

add_executable(dbc_basic main.cpp)
target_link_libraries(dbc_basic PRIVATE dbcparser)

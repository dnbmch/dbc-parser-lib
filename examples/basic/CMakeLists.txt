cmake_minimum_required(VERSION 3.14)
project(dbc_basic_example LANGUAGES CXX)

# Pull in dbc-parser as a subdirectory.
# Adjust the path if you place this example elsewhere.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/dbcparser)

add_executable(dbc_basic main.cpp)
target_link_libraries(dbc_basic PRIVATE dbcextract)

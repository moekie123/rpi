cmake_minimum_required (VERSION 3.21.0 )

project(Controller VERSION 1.0 LANGUAGES CXX)

message("Configure Actuator Controller Project")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

message("Build Actuator Controller Project")
add_executable(Controller main.cpp)


cmake_minimum_required (VERSION 3.21.0 )

project(Controller VERSION 1.0 DESCRIPTION "Actuator Controller" LANGUAGES CXX )

message("Configure Project")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(include)

add_subdirectory(src)

message("Build Project")
add_executable(Controller main.cpp)

target_link_libraries(Controller controller actuator mosquitto)

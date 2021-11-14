cmake_minimum_required (VERSION 3.21.0 )

execute_process(
	COMMAND git describe --tags --abbrev=0
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_TAG
	OUTPUT_STRIP_TRAILING_WHITESPACE )

project(
	Controller
	VERSION ${GIT_TAG}
	DESCRIPTION "Controller"
	LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_DOCUMENTATION_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)

include(cmake/clang-format.cmake)
include(cmake/doxygen.cmake)
include(cmake/googletest.cmake)

include_directories(include)
add_subdirectory(src)

add_executable(Controller main.cpp)
target_link_libraries(Controller controller client protocol mosquitto pthread)

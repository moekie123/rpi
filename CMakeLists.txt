cmake_minimum_required (VERSION 3.21.0 )

project(
	Controller
	DESCRIPTION "Controller"
	LANGUAGES CXX )

# Load macro's 
include(cmake/repo-info.cmake)
include(cmake/clang-format.cmake)
include(cmake/doxygen.cmake)
include(cmake/googletest.cmake)

# Set project version based on Git Info
set( PROJECT_VERSION ${GIT_TAG} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Create executable
add_executable( ${PROJECT_NAME} "")

# Add source files
target_sources( ${PROJECT_NAME} PRIVATE	main.cpp )

# Set build flags
list( APPEND COMPILER_FLAGS "-Wall" "-Wextra")
message("C++ compiler flags: ${COMPILER_FLAGS}")
target_compile_options( ${PROJECT_NAME} PRIVATE ${COMPILER_FLAGS} )

# Fetch processor information
include(cmake/processor.cmake)

include_directories(include)
add_subdirectory(src)

target_link_libraries( ${PROJECT_NAME} framework mosquitto pthread )

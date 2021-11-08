cmake_minimum_required (VERSION 3.21.0 )

project(Controller VERSION 1.0 DESCRIPTION "Controller" LANGUAGES CXX )

message("Configure Project")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include(cmake/clang-format.cmake)

find_package(GTest REQUIRED)
include(CTest)
function( CREATE_TEST TEST_NAME )
	add_executable( ${TEST_NAME}
		${TEST_NAME}.cpp
	)

	target_link_libraries( ${TEST_NAME} gtestd gtest_maind gmockd pthread )
	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
endfunction()

include_directories(include)

add_subdirectory(src)

message("Build Project")
add_executable(Controller main.cpp)
target_link_libraries(Controller controller client protocol mosquitto pthread)



find_package(GTest REQUIRED)

include(CTest)

function( CREATE_TEST TEST_NAME )
	add_executable( ${TEST_NAME}
		${TEST_NAME}.cpp
	)

	target_link_libraries( ${TEST_NAME} gtestd gtest_maind gmockd pthread )
	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
endfunction()


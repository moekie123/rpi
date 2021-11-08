# additional target to perform clang-format run, requires clang-format

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

set( FILE_TAGS main include src )
set( FILTERED_SOURCE_FILES "" )

foreach ( SOURCE_FILE ${ALL_SOURCE_FILES} )
	foreach( FILE_TAG ${FILE_TAGS} )
		string(FIND "${SOURCE_FILE}" "${FILE_TAG}" FOUND_MATCH)

		if ( NOT ${FOUND_MATCH} EQUAL -1)
			list( APPEND FILTERED_SOURCE_FILES ${SOURCE_FILE} )
		endif ()
	endforeach()
endforeach ()

message( ${FILTERED_SOURCE_FILES} )

add_custom_target(
        clangformat
        COMMAND /usr/bin/clang-format
        -style=file
	#	--verbose
	-i
	${FILTERED_SOURCE_FILES}
)

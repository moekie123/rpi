add_subdirectory( controller )
add_subdirectory( client )
add_subdirectory( protocol )

add_library( framework 
	SHARED
	$<TARGET_OBJECTS:controller-objs>
	$<TARGET_OBJECTS:client-objs>
	$<TARGET_OBJECTS:protocol-objs>
)

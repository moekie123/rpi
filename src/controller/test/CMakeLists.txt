enable_testing()

create_test( ControllerTest )
target_link_libraries( ControllerTest 
	#controller 
	#client 
	#protocol 
	#mosquitto 
	)

create_test( ControllerFactoryTest )
target_link_libraries( ControllerFactoryTest 
	controller 
	client 
	protocol 
	mosquitto 
	)

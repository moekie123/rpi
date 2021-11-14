enable_testing()

create_test( ProtocolFactoryTest )
target_link_libraries( ProtocolFactoryTest 
	controller 
	client 
	protocol 
	mosquitto 
	)

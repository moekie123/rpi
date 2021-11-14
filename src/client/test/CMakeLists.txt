enable_testing()

create_test( ClientTest )
target_link_libraries( ClientTest 
	controller 
	client 
	protocol 
	mosquitto 
	)

create_test( ClientFactoryTest )
target_link_libraries( ClientFactoryTest 
	controller 
	client 
	protocol 
	mosquitto 
	)

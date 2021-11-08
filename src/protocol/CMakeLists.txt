add_library( protocol OBJECT 
	ProtocolFactory.cpp
	MqttProtocol.cpp
        )

add_subdirectory( test )

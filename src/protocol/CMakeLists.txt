add_library( protocol-objs OBJECT 
	ProtocolFactory.cpp
	MqttProtocol.cpp)

add_subdirectory( test )

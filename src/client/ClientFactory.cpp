#include <string>
#include <stdexcept>

#include "Factory.h"
#include "IClient.h"

#include "ClientBase.h"
#include "mqtt/MqttClient.h"

// Forware declaration
template<>
IClient* Factory<IClient>::create( const std::string &, const std::string & );

template<>
IClient* Factory<IClient>::create( const std::string& type )
{
	return Factory<IClient>::create( type, "client" );
}

template<>
IClient* Factory<IClient>::create( const std::string& type, const std::string& prefix )
{
	IClient* client;

	if( type.find("client") != std::string::npos )
	{
		client = new ClientBase( prefix + "/base" );
	}
	else if( type.find("mqtt") != std::string::npos )
	{
		client = new MqttClient( prefix + "/mqtt" );
	}

	if( !client ) throw std::runtime_error("failed to create client");

	return client;
}

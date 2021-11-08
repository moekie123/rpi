#include <string>
#include <stdexcept>

#include "Factory.h"
#include "IClient.h"
#include "IProtocol.h"

#include "ClientBase.h"

// Forware declaration
template<>
IClient* Factory<IClient>::create( const std::string &, const std::string & );

template<>
IClient* Factory<IClient>::create( const std::string& type )
{
	return Factory<IClient>::create( type, "client" );
}

template<>
IClient* Factory<IClient>::create( const std::string& type, const std::string& name )
{
	IClient* client = nullptr;

	if( type.find("MqttController") != std::string::npos )
	{
		Factory<IProtocol>* fProtocol = new Factory<IProtocol>();
		IProtocol* protocol = fProtocol->create( "mqtt", name + "/" + "protocol" );

		client = new ClientBase( name, protocol );
	}

	if( client == nullptr ) 
		throw std::runtime_error("failed to create client");

	return client;
}

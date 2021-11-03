#include <string>
#include <stdexcept>

#include "Logger.h"

#include "Factory.h"
#include "IController.h"

#include "ControllerBase.h"

// Forward declaration
template<>
IController* Factory<IController>::create( const std::string &, const std::string & );

template<>
IController* Factory<IController>::create( const std::string & type )
{
	return Factory<IController>::create( type, "controller" );
}

template<>
IController* Factory<IController>::create( const std::string & type, const std::string & prefix )
{
	IController* controller;

	logger::debug("Controller Factory: create [{}]", type);

	const std::string name = prefix + "/controller";

	if( type.find("MqttController") != std::string::npos )
	{
		Factory<IClient>* fClient = new Factory<IClient>();
		IClient* client = fClient->create( type, name );

		controller = new ControllerBase( name, client );
	}

	if( !controller ) throw std::runtime_error("failed to create controlller");

	return controller;
}

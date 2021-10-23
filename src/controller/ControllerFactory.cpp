#include <string>
#include <stdexcept>

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

	if( type.find("controller") != std::string::npos )
	{
		Factory<IClient>* fClient = new Factory<IClient>();

		IClient* client = fClient->create( "mqtt", prefix );

		controller = new ControllerBase( prefix, client );
	}

	if( !controller ) throw std::runtime_error("failed to create controlller");

	return controller;
}

#include <stdexcept>
#include <string>

#include "Logger.h"

#include "Factory.h"
#include "IController.h"

#include "ControllerBase.h"

//! Entity creator
/*!
 *	\param type The derived class from which the returned base is derived
 *  \param name The name of created object
 *	\return Base class from the created type
 */
template <>
IController *Factory< IController >::create( const std::string &type,
                                             const std::string &name )
{
    IController *controller = nullptr;

    logger::debug( "Controller Factory: create [{}]", type );

    if ( type.find( "MqttController" ) != std::string::npos ) {
        Factory< IClient > *fClient = new Factory< IClient >();

        IClient *client = fClient->create( type, name + "/" + "client" );

        controller = new ControllerBase( name, client );
    }

    if ( controller == nullptr )
        throw std::runtime_error( "failed to create controlller" );

    return controller;
}                                         

//! Entity creator
/*!
 *	\param type The derived class from which the returned base is derived
 *	\return Base class from the created type
 */
template <>
IController *Factory< IController >::create( const std::string &type )
{
    return Factory< IController >::create( type, "controller" );
}



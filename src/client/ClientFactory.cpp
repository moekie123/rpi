#include <stdexcept>
#include <string>

#include "Factory.h"
#include "IClient.h"
#include "IProtocol.h"

#include "ClientBase.h"

//! Entity creator
/*!
 *	\param type The derived class from which the returned base is derived
 *  \param name The name of created object
 *	\return Base class from the created type
 */
template <>
IClient *Factory< IClient >::create( const std::string &type,
                                     const std::string &name )
{
    IClient *client = nullptr;

    if ( type.find( "MqttController" ) != std::string::npos ) {
        Factory< IProtocol > *fProtocol = new Factory< IProtocol >();
        IProtocol *protocol =
            fProtocol->create( "mqtt", name + "/" + "protocol" );

        client = new ClientBase( name, protocol );
    }

    if ( client == nullptr )
        throw std::runtime_error( "failed to create client" );

    return client;
}

//! Entity creator
/*!
 *	\param type The derived class from which the returned base is derived
 *	\return Base class from the created type
 */
template <> IClient *Factory< IClient >::create( const std::string &type )
{
    return Factory< IClient >::create( type, "client" );
}


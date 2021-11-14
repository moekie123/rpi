#include <stdexcept>
#include <string>

#include "Factory.h"
#include "IProtocol.h"

#include "MqttProtocol.h"

//! Entity creator
/*!
 *	\param type The derived class from which the returned base is derived
 *  \param name The name of created object
 *	\return Base class from the created type
 */
template <>
IProtocol *Factory< IProtocol >::create( const std::string &type,
                                         const std::string &name )
{
    IProtocol *protocol = nullptr;

    if ( type.find( "mqtt" ) != std::string::npos ) {
        protocol = new MqttProtocol( name );
    }

    if ( protocol == nullptr )
        throw std::runtime_error( "failed to create protocol" );

    return protocol;
}

//! Entity creator
/*!
 *	\param type The derived class from which the returned base is derived
 *	\return Base class from the created type
 */
template <> IProtocol *Factory< IProtocol >::create( const std::string &type )
{
    return Factory< IProtocol >::create( type, "protocol" );
}



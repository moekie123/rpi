#include <stdexcept>
#include <string>

#include "Factory.h"
#include "IProtocol.h"

#include "MqttProtocol.h"

// Forware declaration
template <>
IProtocol *Factory< IProtocol >::create( const std::string &,
                                         const std::string & );

template <> IProtocol *Factory< IProtocol >::create( const std::string &type )
{
    return Factory< IProtocol >::create( type, "protocol" );
}

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

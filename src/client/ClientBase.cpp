#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"

#include "ClientBase.h"

ClientBase::ClientBase()
	:ClientBase("/")
{
}

ClientBase::ClientBase( const std::string name ):
	name( name )
{
	logger::trace( this->name + ": construct" );
}

ClientBase::~ClientBase()
{
	logger::trace( this->name + ": destruct" );
}

void ClientBase::start()
{
	logger::info( this->name + ": start" );
}

void ClientBase::stop()
{
	logger::info( this->name + ": stop" );
}

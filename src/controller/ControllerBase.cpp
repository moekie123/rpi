#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"

#include "ControllerBase.h"

IClient* client;
ControllerBase::ControllerBase( IClient* client ): 
	ControllerBase("/", client)
{
}

ControllerBase::ControllerBase( const std::string name, IClient* client ): 
	name( name )
{
	logger::trace( this->name + ": construct" );

	this->client = client;
	client->attach( this );
}


ControllerBase::~ControllerBase()
{
	logger::trace( this->name + ": destruct");
	delete client;
}

/* Controller Interface */
void ControllerBase::start()
{
	logger::info( this->name + ": start");
	client->start();
}

void ControllerBase::stop()
{
	logger::info("stop");
	client->stop();
}

/* Observer Pattern */
void ControllerBase::update( const std::string &name, const void* data )
{
	logger::info( this->name + ": update " + name );
}

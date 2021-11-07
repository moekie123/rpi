#include "Logger.h"

#include "Factory.h"

#include "ClientBase.h"
#include "ClientState.h"

#include "IProtocol.h"

ClientBase::ClientBase( const std::string name, IProtocol* protocol  )
{
	logger::trace( name + ": construct" );

	this->name = name;

	ClientState::prefix = this->name + "/fsm";

	protocol->attach( this );
	ClientState::accept( protocol );
}

ClientBase::~ClientBase()
{
	logger::trace( this->name + ": destruct" );
}

/* Client Interface */
void ClientBase::start()
{
	logger::info( this->name + ": start" );

	this->running = true;

	ClientState::start();

	eStartup es;
	ClientState::dispatch(es);
}

void ClientBase::stop()
{
	logger::info( this->name + ": stop" );

	eTerminate et;
	ClientState::dispatch(et);
}

/* Observer */
void ClientBase::update( const std::string& cmd, const void* )
{
	logger::trace( this->name + ": " + cmd );

	if( cmd.compare("configured") == 0 )
	{
		// No need for action
	}
	else if( cmd.compare("connected") == 0 )
	{
		eConnected e;
		ClientState::dispatch(e);
	}
	else if( cmd.compare("disconnected") == 0 )
	{
		eDisconnected e;
		ClientState::dispatch(e);
	}
	else if( cmd.compare("destroyed") == 0 )
	{
		this->running = false;
		notify("destroyed", nullptr);
	}
}

#pragma once

#include <tinyfsm/tinyfsm.hpp>
#include <string>
#include <stdexcept>

#include "Runnable.h"
#include "Observable.h"
#include "Observer.h"

#include "IProtocol.h"

/* States */
struct sUninitialized;
struct sConnecting;
struct sIdle;
struct sDisconnecting;
struct sTerminated;
struct sError;

/* Events */
struct eStartup : tinyfsm::Event { };
struct eTerminate : tinyfsm::Event { };
struct eConnected : tinyfsm::Event { };
struct eDisconnected : tinyfsm::Event { };
struct eRetry : tinyfsm::Event { };

/* Client Base State */
class ClientState:
	public tinyfsm::Fsm<ClientState>,
	public Observable
{

public:
	ClientState( const std::string _name ): name( _name ){}

	/* State transitions */
 	virtual inline void entering(){};
	void entry( void ) 
	{
		logger::trace( prefix + "/" + name + ": entry");

		retryCntr = 0;

		entering();
	}

	virtual inline void exiting(){};
	void exit( void )
	{
		logger::trace( prefix + "/" + name + ": exit");
		exiting();
	};

	/* Event bases */
	virtual void react( const tinyfsm::Event & )
	{
		throw std::logic_error("not implemented");
	};

	virtual void react( const eStartup & )
	{
		throw std::logic_error("not implemented");
	};

	virtual void react( const eConnected & )
	{
		throw std::logic_error("not implemented");
	};

	virtual void react( const eDisconnected & )
	{
		throw std::logic_error("not implemented");
	};

	virtual inline void retry(){};
	void react( const eRetry & )
	{
		logger::trace( prefix + "/" + name + ": retry [{}/{}]", retryCntr, retryMax );
		
		if( (retryCntr > retryMax) ) 
			ClientState::transit<sError>();
		else
			retry();

		retryCntr++;
	}

	virtual inline void terminate(){};
	void react( const eTerminate & )
	{
		logger::trace( prefix + "/" + name + ": eTerminate");
		terminate();
	};

	static void accept( IProtocol* protocol )
	{
		logger::trace( prefix + ": accept protocol: {}", fmt::ptr( protocol ));
		
		ClientState::protocol = protocol;
	}

	static inline std::string prefix = "/";

protected:
	// Configuration
	const std::string name;
	static inline IProtocol* protocol = nullptr;

private:
	int retryMax = 3;
	int retryCntr;
};

class  sUninitialized:
	public ClientState
{
public:
	sUninitialized():ClientState("uninitialized"){}

	void react( const eStartup & ) override
	{
		logger::trace( prefix + "/" + name + ": sUninitialized::eStartup" );
		protocol->configure();

		ClientState::transit<sConnecting>();
	};
};

class  sConnecting:
	public ClientState
{
public:
	sConnecting():ClientState("connecting"){}

	void react( const eConnected & ) override
	{
		ClientState::transit<sIdle>();
	}

private:
	inline void entering() override
	{
		protocol->connect();
	};

	void retry() override
	{
		protocol->connect();
	}

	inline void terminate() override
	{
		ClientState::transit<sTerminated>();
	};

};

/* Idle State */
class  sIdle:
	public ClientState
{
public:
	sIdle():ClientState("idle"){}

private:
	inline void terminate() override
	{
		ClientState::transit<sDisconnecting>();
	};
};

/* Disconnecting State */
class  sDisconnecting:
	public ClientState
{
public:
	sDisconnecting():ClientState("disconnecting"){}

	void react( const eDisconnected& ) override
	{
		ClientState::transit<sTerminated>();
	}

private:
	inline void entering() override
	{
		protocol->disconnect();
	};

	void retry() override
	{
		protocol->disconnect();
	}
};

/* Terminate State */
class  sTerminated:
	public ClientState
{
public:
	sTerminated():ClientState("terminated"){}

private:
	inline void entering() override
	{
		protocol->destroy();
	}
};

/* Error State */
class  sError:
	public ClientState
{
public:
	sError():ClientState("error"){}

private:
	inline void terminate() override
	{
		ClientState::transit<sTerminated>();
	};
};

FSM_INITIAL_STATE( ClientState, sUninitialized )


#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <errno.h>

#include <tinyfsm/tinyfsm.hpp>
#include <mosquitto.h>

#include "Logger.h"
#include "MqttClient.h"

struct eCycle : tinyfsm::Event { };
struct eStartup : tinyfsm::Event { };
struct eTerminate : tinyfsm::Event { };

struct sUninitialized;
struct sInitializing;
struct sCreating;
struct sConfiguring;
struct sConnecting;
struct sDestroying;
struct sTerminated;
struct sError;

class MqttClientState:
	public tinyfsm::Fsm<MqttClientState>
{

public:
	
	MqttClientState( const std::string _name): name( _name ){}

	virtual void react( tinyfsm::Event const & ){};
	virtual void react( eCycle const & ){};
	virtual void react( eStartup const & ){};
	virtual void react( eTerminate const & ){};

	virtual void entry( void ){};
  	virtual void exit( void ){};

	static void attach( Observer* observer )
	{
		logger::trace("attach observer: {}", fmt::ptr( observer ));
		observers.push_back( observer );
	}

	static bool isRunning()
	{
		return active;
	}

protected:
	const std::string name;

	inline static bool active = false;
	inline static mosquitto* client = nullptr;

	inline static std::vector< Observer* > observers;
};

/**
* State: Uninitialized
*/
class sUninitialized:
	public MqttClientState
{
	using base = MqttClientState;

public:
	sUninitialized():MqttClientState("uninitialized"){}

private:
	void entry( void ) override
	{
		active = true;
		for( auto observer: observers )
			observer->update(name);
	}
	
	void react( eStartup const & ) override
	{
		logger::trace( name + ": eStartup");
		MqttClientState::transit<sInitializing>();
	};

	void react( eTerminate const & ) override
	{
		logger::trace( name + ": eTerminate");
		MqttClientState::transit<sTerminated>();
	};
};

/**
* State: Initializing
*/
class sInitializing:
	public MqttClientState
{
	using base = MqttClientState;

public:
	sInitializing():MqttClientState("initializing"){}

	void entry( void ) override
	{
		cntr = 0;
		for( auto observer: observers )
			observer->update(name);
	}

	void react( eCycle const & ) override
	{
		logger::trace( name + ": [{}/{}]", cntr, maxRetry );
		cntr++;

		int res =  mosquitto_lib_init();

		if( res == MOSQ_ERR_SUCCESS )
		{
			MqttClientState::transit<sCreating>();
			return;
		}
		else
			logger::error(mosquitto_strerror( res ));
	
		if( cntr > maxRetry )
			MqttClientState::transit<sError>();
	}

	void react( eTerminate const & ) override
	{
		logger::trace( name + ": eTerminate");
		MqttClientState::transit<sDestroying>();
	};

private:
	const int maxRetry = 3;
	int cntr;
};

/**
* State: Creating
*/
class sCreating:
	public MqttClientState
{
	using base = MqttClientState;

public:
	sCreating():MqttClientState("creating"){}

	void entry( void ) override
	{
		cntr = 0;
		for( auto observer: observers )
			observer->update(name);
	}

	void react( eCycle const & ) override
	{
		int res;

		logger::trace( name + ": [{}/{}]", cntr, maxRetry );
		cntr++;

		if( !client )
		{
			client = mosquitto_new( NULL, true, NULL );

			if( client )
				res = MOSQ_ERR_SUCCESS;

			if( res != MOSQ_ERR_SUCCESS )
				logger::error( name + " :" + strerror(errno) );
		}
		else
		{
			res = mosquitto_reinitialise( client, NULL, true, NULL );

			if( res != MOSQ_ERR_SUCCESS )
				logger::error(mosquitto_strerror( res ));
		}

		if( res == MOSQ_ERR_SUCCESS )
			MqttClientState::transit<sConfiguring>();
		else if( cntr > maxRetry )
			MqttClientState::transit<sError>();
	}

	void react( eTerminate const & ) override
	{
		logger::trace( name + ": eTerminate");
		MqttClientState::transit<sDestroying>();
	};

private:
	const int maxRetry = 3;
	int cntr;
};

/**
* State: Configuring
*/
class sConfiguring:
	public MqttClientState
{
	using base = MqttClientState;

public:
	sConfiguring():MqttClientState("configuring"){}

	void entry( void ) override
	{
		cntr = 0;
		for( auto observer: observers )
			observer->update(name);
	}

	void react( eCycle const & ) override
	{
		int res;

		logger::trace( name + ": [{}/{}]", cntr, maxRetry );
		cntr++;

		// Configure username and password
		const std::string username = "admin";
		const std::string password = "password";

		res = mosquitto_username_pw_set( client, username.c_str(), password.c_str() );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
		}

		if( res == MOSQ_ERR_SUCCESS )
			MqttClientState::transit<sConnecting>();

		else if( cntr > maxRetry )
			MqttClientState::transit<sError>();
	}

	void react( eTerminate const & ) override
	{
		logger::trace( name + ": eTerminate");
		MqttClientState::transit<sDestroying>();
	};

private:
	const int maxRetry = 3;
	int cntr;
};

/**
* State: Connecting
*/
class sConnecting:
	public MqttClientState
{
	using base = MqttClientState;

public:
	sConnecting():MqttClientState("connecting"){}

	void entry( void ) override
	{
		cntr = 0;
		for( auto observer: observers )
			observer->update(name);
	}

	void react( eCycle const & ) override
	{
		int res;

		logger::trace( name + ": [{}/{}]", cntr, maxRetry );
		cntr++;

		std::string host = "127.0.0.1";
		int port = 1883;
		res = mosquitto_connect( client, host.c_str(), port, 5 );

		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
		}

		if( res == MOSQ_ERR_SUCCESS )
			logger::info("connecting succes");

		else if( cntr > maxRetry )
			MqttClientState::transit<sError>();
	}

	void react( eTerminate const & ) override
	{
		logger::trace( name + ": eTerminate");
		MqttClientState::transit<sDestroying>();
	};

private:
	const int maxRetry = 3;
	int cntr;
};


/**
* State: Destroying
*/
class sDestroying:
	public MqttClientState
{
	using base = MqttClientState;

public:
	sDestroying():MqttClientState("destroying"){}

	void entry( void ) override
	{
		cntr = 0;

		for( auto observer: observers )
			observer->update(name);
	}

	void react( eCycle const & ) override
	{
		logger::trace( name + ": [{}/{}]", cntr, maxRetry );
		cntr++;

		// Destory
		if( client )
			mosquitto_destroy( client );

		// Cleanup
		int res =  mosquitto_lib_cleanup();
		if( res == MOSQ_ERR_SUCCESS )
		{
			MqttClientState::transit<sTerminated>();
			return;
		}
		else
			logger::error(mosquitto_strerror( res ));

		if( cntr > maxRetry )
			MqttClientState::transit<sError>();
	}

private:
	const int maxRetry = 3;
	int cntr;
};

/**
 * State: Terminated
 */
class sTerminated: 
	public MqttClientState
{

public:
	sTerminated():MqttClientState("terminated"){}

private:
	void entry() override
	{
		for( auto observer: observers )
			observer->update(name);

		active = false;
		for( auto observer: observers )
			observer->halted();
	}
};

/**
 * State: Error
 */
class sError: 
	public MqttClientState
{

public:
	sError():MqttClientState("error"){}

private:
	void entry() override
	{
		logger::error("MqttClient in error");

		for( auto observer: observers )
			observer->update(name);
	}

	void react( eTerminate const & ) override
	{
		MqttClientState::transit<sTerminated>();
	};
};


FSM_INITIAL_STATE( MqttClientState, sUninitialized )

MqttClient::MqttClient()
{
	logger::trace("construct");
	
	int major, minor, revision;
	mosquitto_lib_version( &major, &minor, &revision );
	logger::info("Mosquitto Version: {}.{}.{}", major, minor, revision );

	MqttClientState::start();

	pThread = new std::thread( task );
}

MqttClient::~MqttClient()
{
	logger::trace("destroy");

	if( pThread )
		pThread->join();
}

void MqttClient::run()
{
	logger::debug("run");

	eStartup es;
	MqttClientState::dispatch(es);
}

void MqttClient::halt()
{
	logger::debug("halt");
	
	eTerminate et;
	MqttClientState::dispatch(et);
}

bool MqttClient::isRunning()
{
	return MqttClientState::isRunning();
}

void MqttClient::attach( Observer* observer )
{
	MqttClientState::attach( observer );
}

/* ASync Task */
void MqttClient::task()
{
	logger::info("start task");

	while( true )
	{
		eCycle ec;
		MqttClientState::dispatch(ec);

		if(  !MqttClientState::isRunning() ) break;

		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
	}

	logger::trace("stop task");
}

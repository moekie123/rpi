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

struct eResponseCode : tinyfsm::Event 
{ 
	eResponseCode( int rc ): code( rc ){}
	const int code;
};

struct sUninitialized;
struct sInitializing;
struct sConfiguring;
struct sConnecting;
struct sIdle;
struct sDisconnecting;
struct sDestroying;
struct sTerminated;
struct sError;

void on_connect ( struct mosquitto*, void*, int );
void on_disconnect ( struct mosquitto*, void*, int );

class MqttClientState:
	public tinyfsm::Fsm<MqttClientState>
{

public:
	
	MqttClientState( const std::string _name): name( _name ){}

	virtual void react( tinyfsm::Event const & ){};
	virtual void react( eCycle const & ){};
	virtual void react( eStartup const & ){};
	virtual void react( eTerminate const & ){};
	virtual void react( eResponseCode const & ){};

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
* State: Retry Base
*/
class MqttClientStateBase:
	public MqttClientState
{
public:
	MqttClientStateBase( const std::string name ):MqttClientState( name ){}

	void entry( void ) override
	{
		if( retryEnabled)
			retryCntr = 0;

		entering();
		for( auto observer: observers )
			observer->update(name);
	}

	void react( eCycle const & ) override
	{
		if( retryEnabled )
		{
			logger::trace( name + ": cycle [{}/{}]", retryCntr, retryMax );
			retryCntr++;
		
			bool success = process();
			
			if( success ) return;
			if( (retryCntr > retryMax) ) return;
		
			MqttClientState::transit<sError>();
		}
		else
		{
			logger::trace( name + ": cycle" );
			process();
		}
	}

	void react( eTerminate const & ) override
	{
		logger::trace( name + ": eTerminate");
		terminate();
	};

protected:
	virtual inline void entering(){};
	virtual inline bool process(){ return true; };
	virtual inline void terminate(){};

	bool retryEnabled = true;
	int retryMax = 3;


private:
	int retryCntr;
};


/**
* State: Uninitialized
*/
class sUninitialized:
	public MqttClientStateBase
{
public:
	sUninitialized():MqttClientStateBase("uninitialized")
	{
		retryEnabled = false;
	}

private:
	inline void entering() override
	{
		active = true;
	}
	
	inline bool process() override
	{
		MqttClientState::transit<sInitializing>();
		return true;
	};

	inline void terminate() override
	{
		MqttClientState::transit<sTerminated>();
	};
};

/**
* State: Initializing
* 	In this state only-once setting are set
*/
class sInitializing:
	public MqttClientStateBase
{
public:
	sInitializing():MqttClientStateBase("initializing")
	{
		retryEnabled = false;
	}

	inline bool process() override
	{
		// Initialize Mosquitto Library
		int res =  mosquitto_lib_init();
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error( mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
			return false;
		}

		// Initial Creating
		bool created = false;
		if( !client )
		{
			client = mosquitto_new( NULL, true, NULL );

			if( client )
				created = true;
			else
				logger::error( name + " :" + strerror(errno) );
		}
		// Recreation
		else
		{
			int res = mosquitto_reinitialise( client, NULL, true, NULL );

			if( res == MOSQ_ERR_SUCCESS )
				created = true;
			else
				logger::error(mosquitto_strerror( res ));
		}

		if( !created )
		{
			MqttClientState::transit<sError>();
			return false;
		}

		// Adding callbacks
		mosquitto_connect_callback_set( client, on_connect );
		mosquitto_disconnect_callback_set( client, on_disconnect );

		// Allow Async behaviour
		//	mosquitto_threaded_set(client, true);
		
		// Start Observering Socket data
		res = mosquitto_loop_start( client );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
			return false;
		}

		MqttClientState::transit<sConfiguring>();
		return true;
	}

	inline void terminate() override
	{
		MqttClientState::transit<sDestroying>();
	};
};

/**
* State: Configuring
* 	In this state it is allowed to (re) configure on-the-fly
*/
class sConfiguring:
	public MqttClientStateBase
{
public:
	sConfiguring():MqttClientStateBase("configuring"){}

	inline bool process() override
	{
		// Configure username and password
		const std::string username = "admin";
		const std::string password = "password";

		int res = mosquitto_username_pw_set( client, username.c_str(), password.c_str() );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			return false;
		}

		MqttClientState::transit<sConnecting>();
		return true;
	}

	inline void terminate() override
	{
		MqttClientState::transit<sDestroying>();
	};
};

/**
* State: Connecting
*/
void on_connect ( struct mosquitto* client , void* obj, int rc )
{
	logger::trace("received connect response {}", rc);

	eResponseCode et( rc );
	MqttClientState::dispatch(et);
}

class sConnecting:
	public MqttClientStateBase
{
public:
	sConnecting():MqttClientStateBase("connecting")
	{
		retryEnabled = false;
	}

	inline void entering() override
	{
		connect();
	};

	void react( eResponseCode const& response )
	{
		int rc = response.code;

		if( rc == 0 )
		{
			logger::info("connection accepted");
			MqttClientState::transit<sIdle>();
			return;
		}
		else if( rc == 1 )
			logger::error("connection failure: incorrect protocol");
		else if( rc == 2 )
			logger::error("connection failure: identifier rejected");
		else if( rc == 3 )
			logger::error("connection failure: server unvailable");
		else if( rc == 4 )
			logger::error("connection failure: incorrect credentials");
		else if( rc == 5 )
			logger::error("connection failure: not authorized");

		connect();
	}

	inline void terminate() override
	{
		MqttClientState::transit<sDestroying>();
	};

private:
	const std::string host = "mosquitto";
	const int port = 1883;
	const int keep_alive = 5;

	void connect()
	{
		logger::info("connect to broker: {}:{}", host, port);

		/**
		  * callback 'on_connect' has been attached to mosquitto library
		 */
		int res = mosquitto_connect_async( client, host.c_str(), port, keep_alive );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
		}
	}
};

/**
* State: Idle
*/
class sIdle:
	public MqttClientStateBase
{
public:
	sIdle():MqttClientStateBase("idle"){
		retryEnabled = false;
	}

	inline void terminate() override
	{
		MqttClientState::transit<sDisconnecting>();
	};
};

/**
* State: Disconnecting
*/
void on_disconnect ( struct mosquitto* client , void* obj, int rc )
{
	logger::trace("received disconnect response {}", rc);
	eResponseCode et( rc );
	MqttClientState::dispatch(et);
}

class sDisconnecting:
	public MqttClientStateBase
{
public:
	sDisconnecting():MqttClientStateBase("disconnecting")
	{
		retryEnabled = false;
	}

	inline void entering() override
	{
		disconnect();
	};

	void react( eResponseCode const& response )
	{
		int rc = response.code;

		if( rc == 0 )
			MqttClientState::transit<sDestroying>();
		else
		{
			logger::error("disconnect: invalid response {}", rc );
			disconnect();
		}
	}

	inline void terminate() override
	{
		MqttClientState::transit<sDestroying>();
	};

private:
	void disconnect()
	{
		logger::info("disconnect from broker");

		/**
		 * callback 'on_disconnect' has been attached to mosquitto library
		 */
		int res = mosquitto_disconnect( client );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
		}
	}
};

/**
* State: Destroying
*/
class sDestroying:
	public MqttClientStateBase
{
public:
	sDestroying():MqttClientStateBase("destroying"){}

	inline bool process() override
	{
		// Destory
		if( client )
			mosquitto_destroy( client );

		// Cleanup
		int res =  mosquitto_lib_cleanup();
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			return false;
		}

		MqttClientState::transit<sTerminated>();
		return true;
	}

	inline void terminate() override
	{
		MqttClientState::transit<sDestroying>();
	};
};

/**
 * State: Terminated
 */
class sTerminated: 
	public MqttClientStateBase
{

public:
	sTerminated():MqttClientStateBase("terminated")
	{
		retryEnabled = false;
	}

private:
	
	inline void entering() override
	{
		active = false;
		for( auto observer: observers )
			observer->halted();
	}
};

/**
 * State: Error
 */
class sError: 
	public MqttClientStateBase
{

public:
	sError():MqttClientStateBase("error")
	{
		retryEnabled = false;
	}

private:
	inline void terminate() override
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
	logger::trace("destruct");

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

#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <errno.h>
#include <stdexcept>

#include <tinyfsm/tinyfsm.hpp>
#include <mosquitto.h>

#include "Logger.h"
#include "MqttClient.h"

struct MqttPackage
{
	const int id;
	const std::string topic;
	const std::string payload;
	const int qos;
	const bool retained;
};

/* Mqtt States */
struct sUninitialized;
struct sConnecting;
struct sIdle;
struct sDisconnecting;
struct sTerminated;
struct sError;

/* Mosquitto callbacks */
void on_connect ( struct mosquitto*, void*, int );
void on_disconnect ( struct mosquitto*, void*, int );
void on_log( struct mosquitto*, void*, int, const char* );
void on_publish( struct mosquitto*, void*, int );
void on_subscribe( struct mosquitto*, void*, int, int, const int * );

/* Mqtt Events */
struct eStartup : tinyfsm::Event { };
struct eTerminate : tinyfsm::Event { };
struct eConnected : tinyfsm::Event { };
struct eDisconnected : tinyfsm::Event { };
struct eRetry : tinyfsm::Event { };

struct ePublish : tinyfsm::Event 
{ 
	ePublish( const MqttPackage& _package ): package( _package )
	{
		int res = mosquitto_pub_topic_check( package.topic.c_str() );
		if( res != MOSQ_ERR_SUCCESS )
		{
			std::string msg( mosquitto_strerror( res ));
			logger::error( msg );
			throw std::invalid_argument( msg );
		}
	}
	const MqttPackage package;
};

/* MqttClient Base State */
class MqttClientState:
	public tinyfsm::Fsm<MqttClientState>
{

public:
	
	MqttClientState( const std::string _name ): name( _name ){}

	/* Event bases */
	virtual void react( const tinyfsm::Event & ){};
	virtual void react( const eStartup & ){};
	virtual void react( const eConnected & ){};
	virtual void react( const eDisconnected & ){};
	virtual void react( const ePublish & ){};
	
	virtual void react( const eRetry & )
	{
		logger::trace( name + ": retry [{}/{}]", retryCntr, retryMax );
		
		if( (retryCntr > retryMax) ) 
			MqttClientState::transit<sError>();
		else
			retry();

		retryCntr++;
	}

	virtual void react( const eTerminate & )
	{
		logger::trace( name + ": eTerminate");
		terminate();
	};

	virtual void entry( void ) 
	{
		logger::trace( name + ": entry");

		retryCntr = 0;

		entering();
	
//		for( auto observer: observers )
//			observer->update(name);
	}

	virtual void exit( void )
	{
		logger::trace( name + ": exit");
	};

	/* Others */	
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
	// Configuration
	const std::string name;
	inline static const std::string identifier = "controller";
	inline static const std::string username = "admin";
	inline static const std::string password = "password";
	
	inline static const MqttPackage lastWill = { 0, "controller", "", 0, false };

	inline static const std::string host = "mosquitto";
	inline static const int port = 1883;
	inline static const int keep_alive = 5;

	inline static bool active = false;
	inline static mosquitto* client = nullptr;

	virtual inline void entering(){};
	virtual inline void retry(){};
	virtual inline void terminate(){};

	inline static std::vector< Observer* > observers;

private:
	int retryMax = 3;
	int retryCntr;
};

/* Mosquitto callbacks */
void on_log( struct mosquitto* client, void* obj, int level, const char* msg )
{
	logger::trace("mosquito lib: {}", msg);
}

void on_message(struct mosquitto* client, void * obj, const struct mosquitto_message* message )
{
	std::string topic( message->topic );
	std::string payload( (char*)message->payload, message->payloadlen );

	logger::info("received message: topic:{} payload:{}", topic, payload);
}

void on_connect ( struct mosquitto* client , void* obj, int rc )
{
	logger::trace("received connect response {}", rc);

	if( rc == 0 )
	{
		logger::info("connection accepted");

		eConnected ec;
		MqttClientState::dispatch(ec);

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
	else
		logger::error("connection failure: unknown errer {}", rc);

	eRetry er;
	MqttClientState::dispatch(er);
}

void on_disconnect ( struct mosquitto* client , void* obj, int rc )
{
	logger::trace("received disconnect response {}", rc);

	if( rc == 0 )
	{
		eDisconnected ed;
		MqttClientState::dispatch(ed);
		return;
	}
	
	logger::error("disconnect: invalid response {}", rc );

	eRetry er;
	MqttClientState::dispatch(er);
}

void on_publish( struct mosquitto* client, void* obj, int mid )
{
	logger::info("published id:{}", mid);
}

void on_subscribe( struct mosquitto* client, void* obj, int mid, int qos_count, const int *qos_grand )
{
	logger::info("subscribed id:{}", mid);
}

/**
* State: Uninitialized
*/
class sUninitialized:
	public MqttClientState
{
public:
	sUninitialized():MqttClientState("uninitialized"){}

private:
	inline void entering() override
	{
		active = true;
	}

	void react( const eStartup & )
	{
		int res;
		
		// Initialize Mosquitto Library
		res =  mosquitto_lib_init();
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error( mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
			return;
		}

		// Initial Creating
		if( !client )
		{
			client = mosquitto_new( identifier.c_str(), true, NULL );

			if( !client )
			{	
				logger::error( name + " :" + strerror(errno) );
				MqttClientState::transit<sError>();
			}
		}
		// Recreation
		else
		{
			int res = mosquitto_reinitialise( client, identifier.c_str(), true, NULL );

			if( res != MOSQ_ERR_SUCCESS )
			{
				logger::error(mosquitto_strerror( res ));
				MqttClientState::transit<sError>();
			}
		}

		// Configure username and password
		logger::info("configure: u[{}] p[{}]", username, password);
		res = mosquitto_username_pw_set( client, username.c_str(), password.c_str() );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			return;
		}

		// Configure last will
		logger::info("configure: topic:{} message:{}", lastWill.topic.c_str(), lastWill.payload.c_str());
		res = mosquitto_will_set( client, lastWill.topic.c_str(), lastWill.payload.size(), lastWill.payload.c_str(), lastWill.qos, lastWill.retained );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			return;
		}

		// Adding callbacks
		mosquitto_log_callback_set( client, on_log );
		mosquitto_connect_callback_set( client, on_connect );
		mosquitto_disconnect_callback_set( client, on_disconnect );
		mosquitto_message_callback_set( client, on_message );
		mosquitto_publish_callback_set( client, on_publish );
		mosquitto_subscribe_callback_set( client, on_subscribe );
	
		
		// Start Observering Socket data
		res = mosquitto_loop_start( client );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
			return;
		}

		MqttClientState::transit<sConnecting>();
	};
};

/**
* State: Connecting
*/
class sConnecting:
	public MqttClientState
{
public:
	sConnecting():MqttClientState("connecting"){}

	inline void entering() override
	{
		connect();
	};

	void react( const eConnected & ) override
	{
		int sock = mosquitto_socket(client);
		logger::info("client socket {}", sock );

		MqttClientState::transit<sIdle>();
	}

	void retry() override
	{
		connect();
	}

	inline void terminate() override
	{
		MqttClientState::transit<sTerminated>();
	};

private:
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
	public MqttClientState
{
public:
	sIdle():MqttClientState("idle"){}

	void react( const ePublish &message ) override
	{
		const MqttPackage& package = message.package;

		int id = package.id;
		int res = mosquitto_publish( client, &id, package.topic.c_str(), package.payload.size(), package.payload.c_str(), package.qos, package.retained );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
		}
	};

	inline void terminate() override
	{
		MqttClientState::transit<sDisconnecting>();
	};
};

/**
* State: Disconnecting
*/
class sDisconnecting:
	public MqttClientState
{
public:
	sDisconnecting():MqttClientState("disconnecting"){}

	inline void entering() override
	{
		disconnect();
	};

	void react( const eDisconnected& ) override
	{
		int sock = mosquitto_socket(client);
		logger::info("client socket {}", sock );

		std::this_thread::sleep_for( std::chrono::seconds( keep_alive ));

		MqttClientState::transit<sTerminated>();
	}

	void retry() override
	{
		disconnect();
	}

private:
	void disconnect()
	{
		int res;
		logger::info("disconnect from broker");

		/**
		 * callback 'on_disconnect' has been attached to mosquitto library
		 */
		res = mosquitto_disconnect( client );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
		}
	}
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
	
	inline void entering() override
	{
		int res;

		logger::info("stop loop");
		res = mosquitto_loop_stop( client, false );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
			return;
		}

		// Destory
		logger::info("destroy mosquitto");
		if( client )
			mosquitto_destroy( client );

		// Cleanup
		logger::info("cleanup library");
		res =  mosquitto_lib_cleanup();
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
			return;
		}

		active = false;

//		for( auto observer: observers )
//			observer->halted();
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
	inline void terminate() override
	{
		MqttClientState::transit<sTerminated>();
	};
};

FSM_INITIAL_STATE( MqttClientState, sUninitialized )

MqttClient::MqttClient( const std::string name ):
	ClientBase( name )
{
	logger::trace( this->name + ": construct");
	
	int major, minor, revision;
	mosquitto_lib_version( &major, &minor, &revision );
	logger::info("Mosquitto Version: {}.{}.{}", major, minor, revision );

	MqttClientState::start();
}

MqttClient::~MqttClient()
{
	logger::trace( this->name + ": destruct");
}

void MqttClient::start()
{
	logger::debug( this->name + ": start");

	eStartup es;
	MqttClientState::dispatch(es);
}

void MqttClient::stop()
{
	logger::debug( this->name + ": stop");
	
	eTerminate et;
	MqttClientState::dispatch(et);
}

/*
void MqttClient::publish()
{
	logger::debug( this->name + ": publish");

	MqttPackage package = { 1, "controller/config", "{'version':1}", 0, false };
	ePublish ep( package );

	MqttClientState::dispatch(ep);
}
*/

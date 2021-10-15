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
struct eDestroy : tinyfsm::Event { };
struct eRetry : tinyfsm::Event { };

struct ePublish : tinyfsm::Event 
{ 
	ePublish( const int _mid, const std::string _topic, const std::string _message, int _qos, bool _retained ):
		topic(_topic), payload(_message), qos(_qos), retained(_retained)
	{
		mid = new int( _mid );

		int res = mosquitto_pub_topic_check( _topic.c_str() );
		if( res != MOSQ_ERR_SUCCESS )
		{
			std::string msg( mosquitto_strerror( res ));
			logger::error( msg );
			throw std::invalid_argument( msg );
		}
	}

	int *mid;
	const std::string topic;
	const std::string payload;
	const int qos;
	const bool retained;
};

/* MqttClient Base State */
class MqttClientState:
	public tinyfsm::Fsm<MqttClientState>
{

public:
	
	MqttClientState( const std::string _name): name( _name ){}

	/* Event bases */
	virtual void react( tinyfsm::Event const & ){};
	virtual void react( eStartup const & ){};
	virtual void react( eConnected const & ){};
	virtual void react( eDisconnected const & ){};
	virtual void react( eDestroy const & ){};
	virtual void react( ePublish const & ){};
	
	virtual void react( eRetry const & )
	{
		logger::trace( name + ": retry [{}/{}]", retryCntr, retryMax );
		
		if( (retryCntr > retryMax) ) 
			MqttClientState::transit<sError>();
		else
			retry();

		retryCntr++;
	}

	virtual void react( eTerminate const & )
	{
		logger::trace( name + ": eTerminate");
		terminate();
	};

	/* State transitions */
	virtual void entry( void ) 
	{
		logger::trace( name + ": entry");

		retryCntr = 0;

		entering();
	
		for( auto observer: observers )
			observer->update(name);
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
	const std::string name;

	inline static bool active = false;
	inline static mosquitto* client = nullptr;

	inline static std::vector< Observer* > observers;

	virtual inline void entering(){};
	virtual inline void retry(){};
	virtual inline void terminate(){};

	int retryMax = 3;

private:
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

	void react( eStartup const & )
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
		const std::string identifier = "controller";

		bool created = false;
		if( !client )
		{
			client = mosquitto_new( identifier.c_str(), true, NULL );

			if( client )
				created = true;
			else
				logger::error( name + " :" + strerror(errno) );
		}
		// Recreation
		else
		{
			int res = mosquitto_reinitialise( client, identifier.c_str(), true, NULL );

			if( res == MOSQ_ERR_SUCCESS )
				created = true;
			else
				logger::error(mosquitto_strerror( res ));
		}

		if( !created )
		{
			MqttClientState::transit<sError>();
			return;
		}

		// Allow Async behaviour
		//mosquitto_threaded_set(client, true);

		// Configure username and password
		const std::string username = "admin";
		const std::string password = "password";

		logger::info("configure: username:{} password:{}", username, password);
		res = mosquitto_username_pw_set( client, username.c_str(), password.c_str() );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			return;
		}

		// Configure last will
		const std::string topic = "controller";
		const std::string msg = "";
		const int qos = 0;
		const bool retain = false;

		logger::info("configure: topic:{} message:{}", topic.c_str(), msg.c_str());
		res = mosquitto_will_set( client, topic.c_str(), msg.size(), msg.c_str(), qos, retain );
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

	inline void terminate() override
	{
		MqttClientState::transit<sTerminated>();
	};
};

/**
* State: Connecting
*/
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

	eRetry er;
	MqttClientState::dispatch(er);
}

class sConnecting:
	public MqttClientState
{
public:
	sConnecting():MqttClientState("connecting"){}

	inline void entering() override
	{
		connect();
	};

	void react( eConnected const & ) override
	{
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

/*
int mosquitto_publish(struct mosquitto * mosq, int *mid, const char * topic, int payloadlen, const void * payload, int qos, bool retain	)
bmosq_EXPORT int mosquitto_subscribe( struct mosquitto * mosq, int * mid, const char * sub, int qos )
*/

void on_publish( struct mosquitto* client, void* obj, int mid )
{
	logger::info("published id:{}", mid);
}

void on_subscribe( struct mosquitto* client, void* obj, int mid, int qos_count, const int *qos_grand )
{
	logger::info("subscribed id:{}", mid);
}

class sIdle:
	public MqttClientState
{
public:
	sIdle():MqttClientState("idle"){}

	void react( ePublish const &message ) override
	{
		int res = mosquitto_publish(client, message.mid, message.topic.c_str(), message.payload.size(), message.payload.c_str(), message.qos, message.retained );
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
		}

		// DEBUG
		MqttClientState::transit<sDisconnecting>();

	};

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

	if( rc == 0 )
	{
		eDisconnected ed;
		MqttClientState::dispatch(ed);
	}
	else
	{
		logger::error("disconnect: invalid response {}", rc );
		eRetry er;
		MqttClientState::dispatch(er);
	}
}

class sDisconnecting:
	public MqttClientState
{
public:
	sDisconnecting():MqttClientState("disconnecting"){}

	inline void entering() override
	{
		disconnect();
	};

	void react( eDisconnected const& ) override
	{
		MqttClientState::transit<sTerminated>();
	}

	void retry() override
	{
		disconnect();
	}

	inline void terminate() override
	{
		MqttClientState::transit<sTerminated>();
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

		active = false;
		for( auto observer: observers )
			observer->halted();
	}
	
	void react( eDestroy const & ) override
	{
		int res;

		// For some reason this fails
		/*
		res = mosquitto_loop_stop( client, true);
		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			MqttClientState::transit<sError>();
			return;
		}
		*/

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

	};
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

MqttClient::MqttClient()
{
	logger::trace("construct");
	
	int major, minor, revision;
	mosquitto_lib_version( &major, &minor, &revision );
	logger::info("Mosquitto Version: {}.{}.{}", major, minor, revision );

	MqttClientState::start();
}

MqttClient::~MqttClient()
{
	logger::trace("destruct");

	tinyfsm::Event er;
	MqttClientState::dispatch(er);

}

void MqttClient::run()
{
	logger::debug("run");

	eStartup es;
	MqttClientState::dispatch(es);
}

void MqttClient::publish()
{
	logger::debug("publish");
	
	ePublish ep(1, "controller/config", "{'version':1}", 0, false);
	MqttClientState::dispatch(ep);
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

#include <chrono>

#include "Logger.h"

#include "MqttProtocol.h"

/* Mosquitto callbacks */
void on_connect ( struct mosquitto*, void*, int );
void on_disconnect ( struct mosquitto*, void*, int );
void on_log( struct mosquitto*, void*, int, const char* );
void on_publish( struct mosquitto*, void*, int );
void on_subscribe( struct mosquitto*, void*, int, int, const int * );
void on_message(struct mosquitto* client, void * obj, const struct mosquitto_message* message );

MqttProtocol::MqttProtocol()
	:MqttProtocol("/")
{
}

MqttProtocol::MqttProtocol( const std::string name ):
	name( name )
{
	logger::trace( this->name + ": construct" );
}

MqttProtocol::~MqttProtocol()
{
	logger::trace( this->name + ": destruct" );
}

void MqttProtocol::configure()
{
	int res;
	
	// Initialize Mosquitto Library
	res =  mosquitto_lib_init();
	if( res != MOSQ_ERR_SUCCESS )
	{
		logger::error( mosquitto_strerror( res ));
		notify("error", nullptr);
		return;
	}

	// Initial Creating
	if( !client )
	{
		client = mosquitto_new( identifier.c_str(), true, (void*)this );

		if( !client )
		{	
			logger::error(  strerror(errno) );
			notify("error", nullptr);
		}
	}
	// Recreation
	else
	{
		int res = mosquitto_reinitialise( client, identifier.c_str(), true, NULL );

		if( res != MOSQ_ERR_SUCCESS )
		{
			logger::error(mosquitto_strerror( res ));
			notify("error", nullptr);
		}
	}

	// Configure username and password
	logger::info( this->name + ": configure: u[{}] p[{}]", username, password);
	res = mosquitto_username_pw_set( client, username.c_str(), password.c_str() );
	if( res != MOSQ_ERR_SUCCESS )
	{
		logger::error(mosquitto_strerror( res ));
		notify("error", nullptr);
		return;
	}

	// Configure last will
	logger::info( this->name + ": configure topic:{} message:{}", lastWill.topic.c_str(), lastWill.payload.c_str());
	res = mosquitto_will_set( client, lastWill.topic.c_str(), lastWill.payload.size(), lastWill.payload.c_str(), lastWill.qos, lastWill.retained );
	if( res != MOSQ_ERR_SUCCESS )
	{
		logger::error(mosquitto_strerror( res ));
		notify("error", nullptr);
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
		notify("error", nullptr);
		return;
	}
}

void MqttProtocol::connect()
{
	int res = mosquitto_connect_async( client, host.c_str(), port, keep_alive );
	if( res != MOSQ_ERR_SUCCESS )
	{
		logger::error(mosquitto_strerror( res ));
		notify("error", nullptr);
	}
}

void MqttProtocol::disconnect()
{
	int res;

	res = mosquitto_disconnect( client );
	if( res != MOSQ_ERR_SUCCESS )
	{
		logger::error(mosquitto_strerror( res ));
		notify("error", nullptr);
	}
}

void MqttProtocol::destroy()
{
	int res;

	logger::info( this->name + ": stop loop");
	res = mosquitto_loop_stop( client, false );
	if( res != MOSQ_ERR_SUCCESS )
	{
		logger::error(mosquitto_strerror( res ));
		notify("error", nullptr);
		return;
	}

	// Destory
	logger::info( this->name + ": destroy mosquitto");
	if( client )
		mosquitto_destroy( client );

	// Cleanup
	logger::info( this->name + ": cleanup library");
	res =  mosquitto_lib_cleanup();
	if( res != MOSQ_ERR_SUCCESS )
	{
		logger::error(mosquitto_strerror( res ));
		notify("error", nullptr);
		return;
	}

	notify("destroyed", nullptr);
}

/* Mosquitto callbacks */
void on_log( struct mosquitto* client, void* obj, int level, const char* msg )
{
	MqttProtocol* p = (MqttProtocol*) obj;

	logger::trace( p->name + ": mosquito lib: {}", msg);
}

void on_message(struct mosquitto* client, void * obj, const struct mosquitto_message* message )
{
	MqttProtocol* p = (MqttProtocol*) obj;

	std::string topic( message->topic );
	std::string payload( (char*)message->payload, message->payloadlen );

	logger::info( p->name + ": received message: topic:{} payload:{}", topic, payload);
}

void on_connect ( struct mosquitto* client , void* obj, int rc )
{
	MqttProtocol* p = (MqttProtocol*) obj;

	logger::trace( p->name + "received connect response {}", rc);

	if( rc == 0 )
	{
		logger::info( p->name + ": connection accepted");
		p->notify("connected", nullptr);
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

	p->notify("retry", nullptr);
}

void on_disconnect ( struct mosquitto* client , void* obj, int rc )
{
	MqttProtocol* p = (MqttProtocol*) obj;

	logger::trace( p->name + ": received disconnect response {}", rc);

	if( rc == 0 )
	{
		std::this_thread::sleep_for( std::chrono::seconds( 5 ));

		p->notify("disconnected", nullptr);
		return;
	}
	
	logger::error("disconnect: invalid response {}", rc );

	p->notify("retry", nullptr);
}

void on_publish( struct mosquitto* client, void* obj, int mid )
{
	MqttProtocol* p = (MqttProtocol*) obj;

	logger::info( p->name + ": published id:{}", mid);
}

void on_subscribe( struct mosquitto* client, void* obj, int mid, int qos_count, const int *qos_grand )
{
	MqttProtocol* p = (MqttProtocol*) obj;

	logger::info( p->name + ": subscribed id:{}", mid);
}


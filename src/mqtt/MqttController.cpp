#include <mosquitto.h>
#include <spdlog/spdlog.h>

#include "MqttController.h"

/* Visitor Callbacks */
bool MqttController::initialize() 
{
	int result;

	spdlog::info("initialize");

	spdlog::debug("init library");
	result = mosquitto_lib_init();
	if( result != MOSQ_ERR_SUCCESS )
	{
		spdlog::error("Failed initializing mosquitto library");
		return false;
	}

	return true;
}

bool MqttController::configure() 
{
	int result;

	std::string tag;
	std::string username, password;

	spdlog::info("configure");

	tag = "pca9685";
	username = "actuator";
	password = "actuator";

	
	spdlog::debug("create client");
	client = mosquitto_new( tag.c_str(), true, 0 );
	if( client == NULL )
	{
		spdlog::error( strerror( errno ));
		return false;
	}

	spdlog::debug("configure client");
	result = mosquitto_username_pw_set( client, username.c_str(), password.c_str() );
	if( result != MOSQ_ERR_SUCCESS )
	{
		spdlog::error( mosquitto_strerror( result ));
		return false;
	}

	return true;
}

bool MqttController::connect() 
{
	int result;

	std::string host;
	int port, keep_alive;

	spdlog::info("connect");

	host = "localhost";
	port = 1883;
	keep_alive = 5;

	spdlog::debug("connecting {}:{}", host, port);
	result = mosquitto_connect( client, host.c_str(), port, keep_alive );
	if( result != MOSQ_ERR_SUCCESS )
	{
		spdlog::error( mosquitto_strerror( result ));
		return false;
	}

	return true;
}

bool MqttController::disconnect() 
{
	int result;

	spdlog::info("disconnect");

	// Disconnect
	result = mosquitto_disconnect( client );
	if( result != MOSQ_ERR_SUCCESS )
	{
		spdlog::error( mosquitto_strerror( result ));
	}

	return true;
}

bool MqttController::terminate() 
{
	int result;

	spdlog::info("terminate");

	spdlog::debug("destroy client");
	mosquitto_destroy( client );

	spdlog::debug("cleanup library");
	result = mosquitto_lib_cleanup();
	if( result != MOSQ_ERR_SUCCESS )
	{
		spdlog::error("Failed cleanup mosquitto library");
		return -1;
	}

	return true;
}

bool MqttController::publish() 
{
	int result;
	std::string topic, message;

	spdlog::info("publish");

	// Publish
	topic = "actuator";
	message = "Hello World";

	result = mosquitto_publish( client, NULL, topic.c_str(), strlen(message.c_str()), message.c_str(), 0, false);
	if( result != MOSQ_ERR_SUCCESS )
	{
		spdlog::error( mosquitto_strerror( result ));
		return false;
	}

	return true;
}

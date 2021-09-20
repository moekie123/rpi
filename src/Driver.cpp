#include <iostream>
#include <fstream>

#include <vector>
#include <filesystem>
#include <sys/stat.h>

#include <mosquitto.h>
#include <spdlog/spdlog.h>

#include "Driver.h"

struct InitializeState;
struct ConfigureState;
struct ConnectState;
struct PublishState;
struct DisconnectState;
struct TerminateState;

void Driver::react( HaltEvent const &) 
{ 
	spdlog::info("Halt");
	halt = true;
};

void Driver::entry() 
{ 
	spdlog::trace("Entry");
	cntr = 0;
};

void Driver::exit()
{   	
	spdlog::trace("Exit");
};

/**
 * State: Initialize
 **/
struct InitializeState : Driver
{
	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Initialize {}", cntr);
		cntr += 1;

		if( cntr >= 3 || halt )
		{
			transit<Idle>();
			return;
		}

		if( this->init_mosquitto() )
			transit<ConfigureState>();

	};
};

bool Driver::init_mosquitto() 
{
	int result;

	spdlog::info("init mosquitto library");

	result = mosquitto_lib_init();
	if( result != MOSQ_ERR_SUCCESS )
	{
		spdlog::error("Failed initializing mosquitto library");
		return false;
	}

	return true;
}

/**
 * State: Configure
 **/
struct ConfigureState: Driver
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Configure {}", cntr);
	cntr += 1;

	if( cntr >= 3 || halt )
	{
		transit<InitializeState>();
		return;
	}

	if( this->configure() )
		transit<ConnectState>();
	
  };
};

bool Driver::configure() 
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

/**
 * State: Connect
 **/
struct ConnectState : Driver
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Connect {}", cntr);
	cntr += 1;

	if( cntr >= 3 || halt )
	{
		transit<TerminateState>();
		return;
	}

	if( this->connect() )
		transit<PublishState>();
	
  };
};

bool Driver::connect() 
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

/**
 * State: Disconnect
 **/
struct DisconnectState : Driver
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Disconnect {}", cntr);
	cntr += 1;

	if( cntr >= 3 )
	{
		transit<TerminateState>();
		return;
	}
	
	if( this->disconnect() )
		transit<TerminateState>();
  };
};

bool Driver::disconnect() 
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

/**
 * State: Terminate
 **/
struct TerminateState: Driver
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Terminate {}", cntr);
	cntr += 1;

	if( cntr > 3 )
	{
		transit<Idle>();
		return;
	}

	if( this->terminate() )
		transit<Idle>();
  };
};

bool Driver::terminate() 
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

/**
 * State: Publish
 **/
struct PublishState : Driver
{
  	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Publish {}", cntr);
		cntr += 1;

		if( halt )
		{
			transit<DisconnectState>();
			return;
		}

		if( this->publish() )
		{

		}	
		else
			transit<ConnectState>();
	};
};

bool Driver::publish() 
{
	int result;
	std::string topic, message;

	spdlog::info("publish");

	// PublishState
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

FSM_INITIAL_STATE( Driver, InitializeState )

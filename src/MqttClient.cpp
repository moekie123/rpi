#include <string>
#include <memory>
#include <thread>
#include <chrono>

#include <tinyfsm/tinyfsm.hpp>
#include <mosquitto.h>

#include "Logger.h"
#include "MqttClient.h"

struct eCycle : tinyfsm::Event { };
struct eTerminate : tinyfsm::Event { };

struct sUninitialized;
struct sTerminated;

class MqttClientState:
	public tinyfsm::Fsm<MqttClientState>
{

public:
	
	MqttClientState( const std::string _name):
		name( _name )
	{
	
	}

	virtual void react( tinyfsm::Event const & ){};
	virtual void react( eCycle const & )
	{
		logger::trace( name + ": cycle");
	}

	virtual void react( eTerminate const & )
	{
		logger::trace( name + ": eTerminate");
	};

	virtual void entry(void){
		logger::trace( name + ": entry");
	};

  	virtual void exit(void){
		logger::trace( name + ": exit");
	};

	static void attach( Observer* observer )
	{
		logger::trace("attach observer: {}", fmt::ptr( observer ));
		observers.push_back( observer );
	}

protected:
	const std::string name;
	inline static std::vector< Observer* > observers;
};

class sUninitialized:
	public MqttClientState
{
	using base = MqttClientState;

public:
	sUninitialized():MqttClientState("uninitialized"){}

private:
	void react( eTerminate const & ) override
	{
		logger::trace( name + ": eTerminate");
		MqttClientState::transit<sTerminated>();
	};
};

class sTerminated: 
	public MqttClientState
{

public:
	sTerminated():MqttClientState("terminated"){}

private:
	void entry(void) override
	{
		logger::trace( name + ": entry");

		for( auto observer: observers )
			observer->halted();
	};
};

FSM_INITIAL_STATE( MqttClientState, sUninitialized )

MqttClient::MqttClient()
{
	logger::trace("construct");
	
	int major, minor, revision;
	mosquitto_lib_version( &major, &minor, &revision );
	logger::info("Mosquitto Version: {}.{}.{}", major, minor, revision );
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

	MqttClientState::start();

	pThread = new std::thread( task );
}

void MqttClient::halt()
{
	logger::debug("halt");
	
	eTerminate et;
	MqttClientState::dispatch(et);
}

bool MqttClient::isRunning()
{
	// Anything else but terminated implies the statemachine is running
	return !MqttClientState::is_in_state<sTerminated>();
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
		if( MqttClientState::is_in_state<sTerminated>() ) break;

		eCycle ec;
		MqttClientState::dispatch(ec);

		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
	}

	logger::trace("stop task");
}

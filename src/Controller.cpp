#include <chrono>

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "Controller.h"

#include "mqtt/MqttStateMachine.h"

Controller::Controller()
{
	spdlog::debug("construct");
}

Controller::~Controller()
{
	spdlog::debug("destruct");
}

void Controller::run()
{
	spdlog::info("run");

	CycleEvent event;
	
	MqttStateMachine::start();

	while( true )
	{
		spdlog::trace("cycle");

		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
		
		if( MqttStateMachine::is_in_state<Idle>() ) break;

		MqttStateMachine::dispatch( event );
	}
}

void Controller::halt()
{
	spdlog::info("halt");

	HaltEvent event;
	MqttStateMachine::dispatch( event );
}

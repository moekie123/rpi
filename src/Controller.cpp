#include <chrono>

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "Controller.h"
#include "Channel.h"

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
	
	Channel::start();

	while( true )
	{
		spdlog::trace("cycle");

		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
		
		if( Channel::is_in_state<Idle>() ) break;

		Channel::dispatch( event );
	}
}

void Controller::halt()
{
	spdlog::info("halt");

	HaltEvent event;
	Channel::dispatch( event );
}

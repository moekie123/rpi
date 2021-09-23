#include <chrono>

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "Controller.h"
#include "Channel.h"

using channels = tinyfsm::FsmList<
//	Channel<0,14>,
	Channel<0,15>
	>;

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
	
	channels::start();

	while( true )
	{
		spdlog::trace("cycle");

		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
		
		if( Channel<0,15>::is_in_state<Idle<0,15>>() ) break;

		channels::dispatch( event );
	}
}

void Controller::halt()
{
	spdlog::info("halt");

	HaltEvent event;
	channels::dispatch( event );
}

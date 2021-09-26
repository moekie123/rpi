#include <chrono>

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "Controller.h"
#include "Device.h"

using devices = DeviceList<
	Device<0,14>,
	Device<0,15>
	>;

Controller::Controller()
{
	spdlog::trace("construct");
}

Controller::~Controller()
{
	spdlog::trace("destruct");
}

void Controller::run()
{
	spdlog::info("run");

	CycleEvent event;

	devices::attach( this );
	devices::start();

	while( true )
	{
		spdlog::trace("cycle");

		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
		
		if( Device<0,14>::is_in_state<Idle<0,14>>() ) break;

		devices::dispatch( event );
	}
}

void Controller::halt()
{
	spdlog::info("halt");

	HaltEvent event;
	devices::dispatch( event );
}

void Controller::update( int chip, int channel, const std::string& topic, const std::string& message )
{
	spdlog::trace("update: {} {} {} {}", chip, channel, topic, message);
}

#include <cerrno>
#include <cstring>
#include <signal.h>
#include <chrono>

#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"
#include "Controller.h"

Controller* controller;

enum SystemSignal
{
	start = 0,
	stop = 1,
	destroy = 2
};

SystemSignal sysSignal = start;

void terminate( int signal )
{
	INFO("terminate");

	switch( sysSignal )
	{
		case SystemSignal::start:
			sysSignal = SystemSignal::stop;
			controller->halt();
			break;
		case SystemSignal::stop:
			sysSignal = SystemSignal::destroy;
			break;
		case SystemSignal::destroy:
		default:
			break;
	}
}

int main( int argc, char *argv[] )
{
	spdlog::set_level( spdlog::level::trace );
	spdlog::set_pattern("[%E] %v");
	
	INFO("booting application");

	struct sigaction sigIntHandler;

   	sigIntHandler.sa_handler = terminate;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	
	sigaction( SIGINT, &sigIntHandler, NULL );

	controller = new Controller();
	controller->run();

	while( true )
	{
		TRACE("cycle");
		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));

		if( sysSignal  == SystemSignal::destroy ) break;
	}

	delete controller;

	INFO("terminate application");

	return 0;
}


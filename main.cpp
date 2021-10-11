#include <cerrno>
#include <cstring>
#include <signal.h>
#include <chrono>

#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"
#include "Controller.h"

Controller* controller;

void terminate( int signal )
{
	INFO("terminate");
	controller->halt();
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
	}

	delete controller;

	INFO("terminate application");

	return 0;
}


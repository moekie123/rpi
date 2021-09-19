#include <cerrno>
#include <cstring>
#include <signal.h>

#include <spdlog/spdlog.h>

#include "Controller.h"

Controller* controller;

void terminate( int signal )
{
	spdlog::info("Terminate");
	controller->halt();
}

int main( int argc, char *argv[] )
{
	spdlog::set_level( spdlog::level::trace );

	spdlog::info("Booting Application");

	struct sigaction sigIntHandler;

   	sigIntHandler.sa_handler = terminate;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	
	sigaction( SIGINT, &sigIntHandler, NULL );

	controller = new Controller();

	controller->run();

	delete controller;

	spdlog::info("Terminate Application");
	return 0;
}

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

/*
init_filesystem() 
{
	spdlog::info("init filesystem");

	const std::string directory = "/sys/class/pwm/pwmchip0/";

	spdlog::debug("driver location {}", directory);
	if( !std::filesystem::exists( directory ) )
	{
		spdlog::error("Failed to find driver: {}", directory);
		return false;
	}

	std::vector<std::string> files = {"npwm", "export", "unexport"};
	for ( auto file : files) 
	{
		spdlog::debug("driver io {}", directory + file );
		if( !std::filesystem::exists( directory + file ) )
		{
			spdlog::error("Failed to find {}", directory + file );
			return false;
		}
	}

	std::ifstream npwmfile( directory + "npwm" );
	std::ofstream exportfile( directory + "export" );
	
	std::string buffer;
	std::getline( npwmfile, buffer );

	int npwm = stoi( buffer );
	for( int n = 0; n < npwm; n++ )
	{
		spdlog::info( "export {}", n );
		exportfile << 0x04;
		spdlog::debug( strerror(errno) );

	}

	exportfile.close();
	return true;
}
*/


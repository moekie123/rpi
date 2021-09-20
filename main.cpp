#include <cerrno>
#include <cstring>
#include <signal.h>

#include <spdlog/spdlog.h>

#include "Controller.h"

#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

//Controller* controller;

void terminate( int signal )
{
	spdlog::info("Terminate");
//	controller->halt();
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

/* ### Sandbox ### */
/*
	spdlog::info("init filesystem");

	const std::string directory = "/sys/class/pwm/pwmchip0/";
	const std::vector<std::string> files = { "", "npwm", "export", "unexport"};

	for ( auto file : files) 
	{
		spdlog::debug("driver io {}", directory + file );
		if( !std::filesystem::exists( directory + file ) )
		{
			spdlog::error("Failed to find {}", directory + file );
			return -1;
		}
	}

	spdlog::info("Open filesystem");
	std::ifstream fsNpwm( directory + "npwm" );
	std::ofstream fsExport( directory + "export" );
	std::ofstream fsUnexport( directory + "unexport" );

	spdlog::info("Operate filesystem");

	int npwm;
	fsNpwm >> npwm;
	spdlog::info("npwm {}", npwm);

	for( int n = 0; n < npwm; n++ )
	{
		spdlog::info( "export {}", n );
		fsExport << n << '\n';
	}


	for( int n = 0; n < npwm; n++ )
	{
		spdlog::info( "unexport {}", n );
		fsUnexport << n << '\n';
	}

	spdlog::info("Close filesystem");
	if( fsNpwm.is_open() )
	{
		spdlog::debug("Close export");
		fsNpwm.close();
	}

	if( fsExport.is_open() )
	{
		spdlog::debug("Close export");
		fsExport.close();
	}

	if( fsUnexport.is_open() )
	{
		spdlog::debug("Close unexport");
		fsUnexport.close();
	}
*/

/*
	controller = new Controller();
	controller->run();
	delete controller;
*/

	spdlog::info("Terminate Application");
	return 0;
}


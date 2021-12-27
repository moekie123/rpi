#include <cerrno>
#include <chrono>
#include <cstring>
#include <signal.h>
#include <unistd.h>

#include "Logger.h"

#include "processor.h"

#include "Factory.h"
#include "IController.h"

IController *controller;

void terminate( int )
{
    logger::info( "terminate" );
    controller->stop();
}

int main( int argc, char **argv )
{
    // Configure signal handler for abort interupt
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = terminate;
    sigemptyset( &sigIntHandler.sa_mask );
    sigIntHandler.sa_flags = 0;

    sigaction( SIGINT, &sigIntHandler, NULL );

    // Configure log level
    spdlog::set_level( spdlog::level::trace );
    spdlog::set_pattern( "[%E] %v" );

    logger::info( "booting application" );

    // Iterate through cmdline arguments
    while ( true ) {
        switch ( getopt( argc, argv, "p" ) ) {
        case 'p':
            logger::debug( "Operating System Name: {}", OS_NAME );
            logger::debug( "Operating System Release: {}", OS_RELEASE );
            logger::debug( "Operating System Version: {}", OS_VERSION );
            logger::debug( "Operating System Platform: {}", OS_PLATFORM );
            logger::debug( "64 Bit Architecture: {}", IS_64BIT );
            logger::debug( "Number of logical cores: {}",
                           NUMBER_OF_LOGICAL_CORES );
            logger::debug( "Number of physical cores: {}",
                           NUMBER_OF_PHYSICAL_CORES );
            logger::debug( "Total virtual memory: {} MB",
                           TOTAL_VIRTUAL_MEMORY );
            logger::debug( "Available virtual memory: {} MB",
                           AVAILABLE_VIRTUAL_MEMORY );
            logger::debug( "Total physical memory: {} MB",
                           TOTAL_PHYSICAL_MEMORY );
            logger::debug( "Available physical memory: {} MB",
                           AVAILABLE_PHYSICAL_MEMORY );
            return 1;
        case -1:
            break;
        default:
            printf( "Help/Usage Example\n" );
            break;
        }
        break;
    }

    // Create Controller
    Factory< IController > *fController = new Factory< IController >();
    controller = fController->create( "MqttController" );

    // Start Controlller
    controller->start();

    while ( controller->isRunning() )
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    delete controller;
    logger::info( "terminate application" );

    return 0;
}

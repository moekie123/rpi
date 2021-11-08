#include <cerrno>
#include <chrono>
#include <cstring>
#include <signal.h>

#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"

#include "Factory.h"
#include "IController.h"

IController *controller;

void terminate( int signal )
{
    logger::info( "terminate" );
    controller->stop();
}

int main( int argc, char *argv[] )
{
    spdlog::set_level( spdlog::level::trace );
    spdlog::set_pattern( "[%E] %v" );

    logger::info( "booting application" );

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = terminate;
    sigemptyset( &sigIntHandler.sa_mask );
    sigIntHandler.sa_flags = 0;

    sigaction( SIGINT, &sigIntHandler, NULL );

    Factory< IController > *fController = new Factory< IController >();
    controller = fController->create( "MqttController" );
    controller->start();

    while ( controller->isRunning() )
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    delete controller;
    logger::info( "terminate application" );

    return 0;
}

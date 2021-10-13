#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"

#include "Controller.h"
#include "MqttClient.h"

MqttClient* client;

Controller::Controller()
{
	logger::trace("construct");

	client = new MqttClient();
	client->attach( this );
}

Controller::~Controller()
{
	logger::trace("destroy");
	delete client;
}

void Controller::run()
{
	logger::info("run");
	client->run();
}

void Controller::halt()
{
	logger::info("halt");
	client->halt();
}

bool Controller::isRunning()
{
	return client->isRunning();
}

/* Observer Pattern */
void Controller::halted()
{
	logger::info("halted");
}

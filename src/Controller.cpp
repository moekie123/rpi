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

	active = true;

	client->run();
}

void Controller::halt()
{
	logger::info("halt");
	client->halt();
}

bool Controller::isRunning()
{
	return active;
}

/* Observer Pattern */
void Controller::halted()
{
	logger::info("halted");
	active = false;
}

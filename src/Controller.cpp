
#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"

#include "Controller.h"
#include "MqttClient.h"

Controller::Controller()
{
	TRACE("construct");
	MqttClient::attach( this );
}

Controller::~Controller()
{
	TRACE("destroy");
}

void Controller::run()
{
	INFO("run");
}

void Controller::halt()
{
	INFO("halt");
}


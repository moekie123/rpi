#pragma once

#include <mosquitto.h>

struct MqttController
{
	public:
		bool initialize();
		bool configure();
		bool connect();
		bool disconnect();
		bool terminate();
		bool publish();

	private:
		mosquitto* client;
};

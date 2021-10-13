#pragma once

#include <memory>
#include <Observer.h>

class MqttClient
{
public:
	MqttClient();
	~MqttClient();

	static void initialize();
	static void terminate();

	static void attach( Observer* );
};

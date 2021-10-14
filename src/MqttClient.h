#pragma once

#include <memory>
#include "Observer.h"

class MqttClient
{
public:
	MqttClient();
	~MqttClient();

	void run();
	void halt();

	void attach( Observer* );

	void publish();

	bool isRunning();

private:
	std::thread* pThread = nullptr;

	inline static bool active = false;
	static void task();
};

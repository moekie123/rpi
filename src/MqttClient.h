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

	bool isRunning();

private:
	std::thread* pThread = nullptr;

	inline static bool active = false;
	static void task();
};

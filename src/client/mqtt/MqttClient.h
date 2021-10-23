#pragma once

#include <string>
#include <memory>

#include "../ClientBase.h"
#include "Observer.h"

class MqttClient:
	public ClientBase
{
public:
	MqttClient( const std::string );
	~MqttClient();

	void attach( Observer* );

	void publish();

	bool isRunning();

	/* Client Interface */
	void start() override;
	void stop() override;

private:
	inline static bool active = false;
};

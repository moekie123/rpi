#pragma once

#include <mosquitto.h>

#include "IProtocol.h"

struct MqttPackage
{
	const int id;
	const std::string topic;
	const std::string payload;
	const int qos;
	const bool retained;
};

class MqttProtocol:
	public IProtocol
{
public:
	MqttProtocol( const std::string );	

	~MqttProtocol();

	void configure() override;
	void connect() override;
	void disconnect() override;
	void destroy() override;

protected:

	// Configuration
	inline static const std::string identifier = "controller";
	inline static const std::string username = "admin";
	inline static const std::string password = "password";
	
	inline static const MqttPackage lastWill = { 0, "controller", "", 0, false };

	inline static const std::string host = "mosquitto";
	inline static const int port = 1883;
	inline static const int keep_alive = 5;

	inline static mosquitto* client = nullptr;
};

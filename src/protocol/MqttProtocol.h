#pragma once

#include <mosquitto.h>

#include "IProtocol.h"

//! Mqtt publish data structure
struct MqttPackage {
	//! The package id
    const int id;

	//! The topic on which the payload should be published
    const std::string topic;

	//! The payload that is published
    const std::string payload;

	//! The Quality of Service
    const int qos;

	//! The retained flag
    const bool retained;
};

//! Concrete Protocol
class MqttProtocol : public IProtocol {
  public:
	//! The constructor
	/*!
	 *	\param name The name of the protocol object
	 */
    MqttProtocol( const std::string name );

    ~MqttProtocol();

	/* IProtocol Methods */
    void configure() override;
    void connect() override;
    void disconnect() override;
    void destroy() override;

  protected:

	//! The client id on the broker
    inline static const std::string identifier = "controller";

	//! Credentials: username for connecting to the mqtt broker
    inline static const std::string username = "admin";

	//! Credentials: password for connecting to the mqtt broker
    inline static const std::string password = "password";

	//! The last will package configuration
	/*!
	 *	We the connection is unexpectedly lost an '\0' character will be published
	 */
    inline static const MqttPackage lastWill = { 0, "controller", "", 0,
                                                 false };

	//! The hostname of the broker
    inline static const std::string host = "mosquitto";

	//! The port of the broker
    inline static const int port = 1883;

	//! The keep alive of the connection
	/*!
	 * The interval, in seconds, in which the client verifies whether the broker is still available
	 */
    inline static const int keep_alive = 5;

	//! The mqtt client
    inline static mosquitto *client = nullptr;
};

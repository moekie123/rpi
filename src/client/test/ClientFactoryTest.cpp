#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "Logger.h"

#include "Factory.h"
#include "IClient.h"

/** Controller Factory
 * 	Create mqtt client
 **/
TEST( Factory, create_mqtt_controller ) 
{
    	//arrange
	Factory<IClient>* fClient = new Factory<IClient>();
	
	// act
	auto* client = fClient->create("MqttController", "client");

	//assert
	EXPECT_TRUE( (client != nullptr) );
}

/** Controller Factory
 * 	Create unknown controller
 **/
TEST( Factory, create_unknown ) 
{
    	//arrange
	Factory<IClient>* fClient = new Factory<IClient>();

	//assert
	EXPECT_ANY_THROW( fClient->create("UnknownType", "client"));
}

int main(int argc, char **argv) 
{
	spdlog::set_level( spdlog::level::warn );
	spdlog::set_pattern("[%E] %v");

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


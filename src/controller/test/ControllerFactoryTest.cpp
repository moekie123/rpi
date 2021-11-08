#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "Logger.h"

#include "Factory.h"
#include "IController.h"

/** Controller Factory
 * 	Create mqtt controller
 **/
TEST( Factory, create_mqtt_controller ) 
{
    	//arrange
	Factory<IController>* fController = new Factory<IController>();
	
	// act
	auto* controller = fController->create("MqttController", "controller");

	//assert
	EXPECT_TRUE( (controller != nullptr) );
}

/** Controller Factory
 * 	Create unknown controller
 **/
TEST( Factory, create_unknown ) 
{
    	//arrange
	Factory<IController>* fController = new Factory<IController>();

	//assert
	EXPECT_ANY_THROW( fController->create("UnknownType", "controller"));
}


int main(int argc, char **argv) 
{
	spdlog::set_level( spdlog::level::warn );
	spdlog::set_pattern("[%E] %v");

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "Logger.h"

#include "Factory.h"
#include "IProtocol.h"

/** Controller Factory
 * 	Create mqtt client
 **/
TEST( Factory, create_mqtt_protocol ) 
{
    	//arrange
	Factory<IProtocol>* fProtocol = new Factory<IProtocol>();
	
	// act
	auto* protocol = fProtocol->create("mqtt", "client");

	//assert
	EXPECT_TRUE( (protocol != nullptr) );
}

/** Controller Factory
 * 	Create unknown controller
 **/
TEST( Factory, create_unknown ) 
{
    	//arrange
	Factory<IProtocol>* fProtocol = new Factory<IProtocol>();

	//assert
	EXPECT_ANY_THROW( fProtocol->create("UnknownType", "client"));
}

int main(int argc, char **argv) 
{
	spdlog::set_level( spdlog::level::warn );
	spdlog::set_pattern("[%E] %v");

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


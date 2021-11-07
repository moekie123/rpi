#include <gtest/gtest.h>

#include "Factory.h"
#include "IController.h"

#include "mocks/ControllerMock.h"

/** Controller Factory
 * 	Create default
 **/
TEST( Factory, create ) 
{
    	//arrange
	Factory<IController>* fController = new Factory<IController>();

	//assert
	EXPECT_ANY_THROW( fController->create("UnknownType", "controller"));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


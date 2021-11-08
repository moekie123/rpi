#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "IController.h"

#include "../ControllerBase.h"
#include "mocks/ClientMock.h"

/** IController Interface
 * 	Runnable method - start 
 **/
TEST( ControllerBase, runnable_start ) 
{
	//arrange
	ClientMock mock;
	auto* controller = new ControllerBase("classUnderTest", &mock);

	EXPECT_CALL( mock, start() );

	//act
	controller->start();
}

/** IController Interface
 * 	Runnable method - stop 
 **/
TEST( ControllerBase, runnable_stop ) 
{
    //arrange
    //act
    //assert
}

/** IController Interface
 * 	Runnable method - isRunning
 **/
TEST( ControllerBase, runnable_verify_running_state ) 
{
    //arrange
    //act
    //assert
}

/** IController Interface
 * 	Observable method - attach
 **/
TEST( ControllerBase, observable_attach ) 
{
    //arrange
    //act
    //assert
}

/** IController Interface
 * 	Observer method - update
 **/
TEST( ControllerBase, observable_update ) 
{
    //arrange
    //act
    //assert
}

int main(int argc, char **argv) 
{
	spdlog::set_level( spdlog::level::warn );
	spdlog::set_pattern("[%E] %v");

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


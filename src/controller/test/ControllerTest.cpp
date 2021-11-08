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
	ClientMock mock;
	auto* controller = new ControllerBase("classUnderTest", &mock);

	EXPECT_CALL( mock, stop() );

	//act
	controller->stop();
}

/** IController Interface
 * 	Runnable method - isRunning - initial state
 **/
TEST( ControllerBase, runnable_verify_running_state_initial ) 
{
	//arrange
	ClientMock mock;
	auto* controller = new ControllerBase("classUnderTest", &mock);

	// act
	auto state = controller->isRunning();

	// assert
	EXPECT_FALSE( state );
}

/** IController Interface
 * 	Runnable method - isRunning - startup state
 **/
TEST( ControllerBase, runnable_verify_running_state_start ) 
{
	//arrange
	ClientMock mock;
	auto* controller = new ControllerBase("classUnderTest", &mock);

	EXPECT_CALL( mock, start() );

	//act
	controller->start();
	auto state = controller->isRunning();

	// assert
	EXPECT_TRUE( state );
}

/** IController Interface
 * 	Runnable method - isRunning - stopping state
 **/
TEST( ControllerBase, runnable_verify_running_state_stop ) 
{
	bool state;

	//arrange
	ClientMock mock;
	auto* controller = new ControllerBase("classUnderTest", &mock);

	// act and assert
	EXPECT_CALL( mock, start() );
	controller->start();

	state = controller->isRunning();
	EXPECT_TRUE( state );

	// act and assert
	controller->update("destroyed", nullptr);

	state = controller->isRunning();
	EXPECT_FALSE( state );
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


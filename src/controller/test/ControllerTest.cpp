#include <gtest/gtest.h>

#include "Factory.h"
#include "IController.h"

#include "mocks/ControllerMock.h"

/** IController Interface
 * 	Runnable method - start 
 **/
TEST( ControllerBase, Runnable_Start ) 
{
    //arrange
    //act
    //assert
}

/** IController Interface
 * 	Runnable method - stop 
 **/
TEST( ControllerBase, Runnable_Stop ) 
{
    //arrange
    //act
    //assert
}

/** IController Interface
 * 	Runnable method - isRunning
 **/
TEST( ControllerBase, Runnable_IsRunning ) 
{
    //arrange
    //act
    //assert
}

/** IController Interface
 * 	Observable method - attach
 **/
TEST( ControllerBase, Observable_IsRunning ) 
{
    //arrange
    //act
    //assert
}

/** IController Interface
 * 	Observer method - update
 **/
TEST( ControllerBase, Observer_Update ) 
{
    //arrange
    //act
    //assert
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


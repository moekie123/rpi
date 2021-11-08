#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "IClient.h"

#include "../ClientBase.h"
#include "mocks/ProtocolMock.h"

/** IClient Interface
 * 	FSM - start 
 **/
TEST( ClientBase, fsm_start ) 
{
	//arrange
	ProtocolMock mock;
	auto* client = new ClientBase("classUnderTest", &mock);

	// Stop sequence
	EXPECT_CALL( mock, configure() );
	EXPECT_CALL( mock, connect() );
	client->start();
}

/** IClient Interface
 * 	FSM - stop 
 **/
TEST( ClientBase, fsm_stop ) 
{
	//arrange
	ProtocolMock mock;
	auto* client = new ClientBase("classUnderTest", &mock);

	// Start sequence
	EXPECT_CALL( mock, configure() );
	EXPECT_CALL( mock, connect() );
	client->start();
	client->update("connected", nullptr);

	// Stop sequence
	EXPECT_CALL( mock, disconnect() );
	EXPECT_CALL( mock, destroy() );
	client->stop();
	client->update("disconnected", nullptr);
}

int main(int argc, char **argv) 
{
	spdlog::set_level( spdlog::level::warn );
	spdlog::set_pattern("[%E] %v");

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


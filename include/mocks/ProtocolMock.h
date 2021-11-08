#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "IProtocol.h"

class ProtocolMock:
	public IProtocol
{
public:
	MOCK_METHOD( void, configure, (), (override));
	MOCK_METHOD( void, connect, (), (override));
	MOCK_METHOD( void, disconnect, (), (override));
	MOCK_METHOD( void, destroy, (), (override));

	// Generic
	MOCK_METHOD( const std::string&, getName, ());

	// Observable
	MOCK_METHOD( void, attach, (Observer*));
	MOCK_METHOD( void, notify, (const std::string&, const void*));
};

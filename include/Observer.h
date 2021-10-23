#pragma once

#include <string>

class Observer
{
public:
	virtual ~Observer(){};

	virtual void update( const std::string& ) = 0;

	virtual void halted() = 0;
};

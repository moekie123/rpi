#pragma once

#include <string>

class Observer
{
public:
	virtual ~Observer(){};

	virtual void update( const std::string&, const void* ) = 0;
};

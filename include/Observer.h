#pragma once

#include <string>

#include "Generic.h"

class Observer:
	public virtual Generic
{
public:
	virtual ~Observer(){};

	virtual void update( const std::string&, const void* ) = 0;
};

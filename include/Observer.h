#pragma once

#include <cstring>

class Observer
{
	public:
		virtual void update( const int, const int, const std::string&, const std::string& ) = 0;
};

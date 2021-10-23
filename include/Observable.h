#pragma once

#include<vector>

#include "Logger.h"
#include "Observer.h"

class Observable
{

public:
	void attach( Observer* observer )
	{
		logger::trace("Observable: attach");
		observers.push_back(observer);
	}

	void notify( const std::string& cmd, const void* data )
	{
		logger::trace("Observable: {}", cmd);

		for( auto observer: observers)
			observer->update( cmd, data );
	}

private:
	std::vector<Observer*> observers;

};

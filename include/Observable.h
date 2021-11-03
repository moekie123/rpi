#pragma once

#include<vector>

#include "Logger.h"

#include "Generic.h"
#include "Observer.h"

class Observable:
	public virtual Generic
{

public:
	void attach( Observer* observer )
	{
		logger::trace( this->name + ": attach observer: {}", fmt::ptr( observer ));
		observers.push_back(observer);
	}

	void notify( const std::string& cmd, const void* data )
	{
		logger::trace( this->name + ": Observable: {}", cmd);

		for( auto observer: observers)
			observer->update( cmd, data );
	}

private:
	std::vector<Observer*> observers;

};

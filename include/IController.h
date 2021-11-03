#pragma once

#include "Generic.h"
#include "Runnable.h"

#include "Observable.h"
#include "Observer.h"

class IController:
	public Generic,
	public Runnable,
	public Observable, 
	public Observer
{

public:
	IController( std::string _test ):
		Generic( test )
	{

	}
};

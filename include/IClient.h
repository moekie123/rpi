#pragma once

#include "Runnable.h"

#include "Observable.h"
#include "Observer.h"

class IClient:
	public Runnable, 
	public Observable,
	public Observer
{

};

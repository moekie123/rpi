#pragma once

#include "Runnable.h"

#include "IProtocol.h"

#include "Observable.h"
#include "Observer.h"

class IClient:
	public Runnable, 
	public Observable,
	public Observer
{

};

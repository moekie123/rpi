#pragma once

#include "Runnable.h"

#include "Observable.h"
#include "Observer.h"

class IController:
	public Runnable,
	public Observable, 
	public Observer
{

};

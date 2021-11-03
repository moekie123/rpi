#pragma once

#include "Generic.h"

class Runnable:
	public virtual Generic
{
public:
	virtual void start() = 0;
	virtual void stop() = 0;

	virtual bool isRunning(){ return running; }

protected:
	bool running = false;
};

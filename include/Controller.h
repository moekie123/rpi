#pragma once

#include "Observer.h"

class Controller:
	public Observer
{
public:
	Controller();
	~Controller();

	void run();
	void halt();

	/* Observer pattern */
};

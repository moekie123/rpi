#pragma once

#include <string>

#include "Observer.h"

class Controller:
	public Observer
{
public:
	Controller();
	~Controller();

	void run();
	void halt();

	bool isRunning();

	/* Observer */
	void halted() override;
};

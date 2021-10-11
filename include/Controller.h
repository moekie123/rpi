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
	void initialized() override;
	void ready() override;
	void terminated() override;

	void error() override;
};

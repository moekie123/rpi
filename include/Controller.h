#pragma once

#include <cstring>

#include "Observer.h"

class Controller:
	public Observer
{
public:
	Controller();
	~Controller();

	void run();
	void halt();

	/* Observer Pattern */
	void update( int, int, const std::string&, const std::string& ) override;

private:
	bool active = false;
};

#pragma once

class Controller
{
public:
	Controller();
	~Controller();

	void run();
	void halt();

private:
	bool active = false;
};

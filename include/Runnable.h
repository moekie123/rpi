#pragma once

class Runnable
{
public:
	virtual void start() = 0;
	virtual void stop() = 0;

	virtual bool isRunning(){ return running; }

protected:
	bool running = false;
};

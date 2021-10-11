#pragma once

class Observer
{

public:
	virtual void initialized() = 0;
	virtual void ready() = 0;
	virtual void terminated() = 0;

	virtual void error() = 0;
};

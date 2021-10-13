#pragma once

class Observer
{
public:
	virtual ~Observer();
	virtual void halted() = 0;
};

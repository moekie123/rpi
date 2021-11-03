#pragma once

#include "Observable.h"

class IProtocol:
	public virtual Generic,
	public Observable
{
public:
	virtual void configure() = 0;
	virtual void connect() = 0;
	virtual void disconnect() = 0;
	virtual void destroy() = 0;
};

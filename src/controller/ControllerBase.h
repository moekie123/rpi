#pragma once

#include <string>

#include "IController.h"
#include "Observer.h"

#include "IClient.h"

class ControllerBase:
	public IController,
	public Observer
{
public:
	ControllerBase( IClient* );
	ControllerBase( const std::string, IClient* );
	~ControllerBase();

	bool isRunning();

	/* Controller Interface */
	void start() override;
	void stop() override;

	/* Observer */
	void update( const std::string & ) override;
	void halted() override;

private:
	const std::string name;
	IClient* client;
};

#pragma once

#include <string>

#include "IController.h"
#include "IClient.h"

class ControllerBase:
	public IController
{
public:
	ControllerBase( const std::string, IClient* );
	~ControllerBase();

	/* Controller Interface */
	void start() override;
	void stop() override;

	/* Observer */
	void update( const std::string &, const void * ) override;

private:
	IClient* client;
};

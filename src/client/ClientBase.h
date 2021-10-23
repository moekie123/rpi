#pragma once

#include <string>

#include "IClient.h"

class ClientBase:
	public IClient
{
public:
	ClientBase();
	ClientBase( const std::string );

	~ClientBase();

	/* Client Interface */
	void start() override;
	void stop() override;

protected:
	const std::string name;
};

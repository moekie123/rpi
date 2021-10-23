#pragma once

#include <string>

#include "IClient.h"
#include "Observable.h"
#include "Observer.h"

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

	/* Observer */
	void update( const std::string&, const void* ) override
	{}

protected:
	const std::string name;
};

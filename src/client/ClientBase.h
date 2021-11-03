#pragma once

#include <string>

#include "IClient.h"

#include "Observable.h"
#include "Observer.h"

class ClientBase:
	public IClient
{
public:
	ClientBase( IProtocol* );
	ClientBase( const std::string, IProtocol* );

	~ClientBase();

	/* Client Interface */
	void start() override;
	void stop() override;

	/* Observer */
	void update( const std::string&, const void* ) override;
};

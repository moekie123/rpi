#pragma once

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "MqttController.h"

struct CycleEvent: tinyfsm::Event{};
struct HaltEvent: tinyfsm::Event{};

struct MqttStateMachine : tinyfsm::Fsm<MqttStateMachine>
{
	public:
		inline static MqttController* visitor = NULL;
		inline static bool halt = false;

		MqttStateMachine()
		{
			if( visitor == NULL )
				visitor = new MqttController();
		};
		
		virtual void react( CycleEvent const &) = 0;
		virtual void react( HaltEvent const &) 
		{ 
			spdlog::info("Halt");
			halt = true;
		};

		virtual void entry() 
		{ 
			spdlog::trace("Entry");
			cntr = 0;
		};

		virtual void exit()
		{   	
			spdlog::trace("Exit");
		};


	protected:
		int cntr;
};

struct Idle;
struct Initialize;
struct Configure;
struct Connect;
struct Publish;
struct Disconnect;
struct Terminate;

// Idle
struct Idle : MqttStateMachine
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Idle {}", cntr);
	cntr += 1;

	if( halt ) return;
  };
};

// Initialize
struct Initialize : MqttStateMachine
{
	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Initialize {}", cntr);
		cntr += 1;

		if( cntr >= 3 || halt )
		{
			transit<Idle>();
			return;
		}

		if( visitor->initialize() )
			transit<Configure>();

	};
};

// Configure
struct Configure: MqttStateMachine
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Configure {}", cntr);
	cntr += 1;

	if( cntr >= 3 || halt )
	{
		transit<Initialize>();
		return;
	}

	if( visitor->configure() )
		transit<Connect>();
	
  };
};


// Connect
struct Connect : MqttStateMachine
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Connect {}", cntr);
	cntr += 1;

	if( cntr >= 3 || halt )
	{
		transit<Terminate>();
		return;
	}

	if( visitor->connect() )
		transit<Publish>();
	
  };
};


// Publish
struct Publish : MqttStateMachine
{
  	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Publish {}", cntr);
		cntr += 1;

		if( halt )
		{
			transit<Disconnect>();
			return;
		}

		if( visitor->publish() )
		{

		}	
		else
			transit<Connect>();
	};
};


// Discconnect
struct Disconnect : MqttStateMachine
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Disconnect {}", cntr);
	cntr += 1;

	if( cntr >= 3 )
	{
		transit<Terminate>();
		return;
	}
	
	if( visitor->disconnect() )
		transit<Terminate>();
  };
};


// Terminate
struct Terminate: MqttStateMachine
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Terminate {}", cntr);
	cntr += 1;

	if( cntr > 3 )
	{
		transit<Idle>();
		return;
	}

	if( visitor->terminate() )
		transit<Idle>();
  };
};


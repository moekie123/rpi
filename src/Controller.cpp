
#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"
#include "Controller.h"

struct eInitialize : tinyfsm::Event { };
struct eInitialized : tinyfsm::Event { };

struct eTerminate : tinyfsm::Event { };
struct eTerminated : tinyfsm::Event { };

struct eError : tinyfsm::Event { };

struct sIdle;
struct sInitializing;
struct sReady;
struct sTerminating;

class ControllerState:
	public tinyfsm::Fsm<ControllerState>
{

public:
	virtual void react( eInitialize const & ){}
	virtual void react( eInitialized const & ){}

	virtual void react( eTerminate const & ){}
	virtual void react( eTerminated const & ){}

	virtual void react( eError const & ){}

	virtual void react( tinyfsm::Event const & )
	{
		DEBUG("react");
	};

	virtual void entry(void)
	{
		DEBUG("entry");
	};

  	virtual void exit(void)
	{
		DEBUG("exit");
	};
};

class sIdle: 
	public ControllerState
{
	using base = ControllerState;

	virtual void react( eInitialize const &) override
	{
		TRACE("initialze");

		// Check all elements
		base::transit< sInitializing >();
	}
};

class sInitializing: 
	public ControllerState
{
	using base = ControllerState;

	virtual void entry(void) override
	{
		// Trigger all elements 
	}

	virtual void react( eInitialized const &) override
	{
		TRACE("initialized");
		base::transit< sReady >();
	}

	virtual void react( eTerminate const &) override
	{
		TRACE("terminate");
		base::transit< sTerminating >();
	}

};

class sReady:
	public ControllerState
{
	using base = ControllerState;

	void react( eTerminate const &) override
	{
		TRACE("terminate");
		base::transit< sTerminating >();
	}
};

class sTerminating: 
	public ControllerState
{
	using base = ControllerState;

	virtual void entry(void) override
	{
		// Trigger all elements 
	}

	virtual void react( eTerminated const &) override
	{
		TRACE("terminated");
	}
};

FSM_INITIAL_STATE( ControllerState, sIdle )

using fsm = ControllerState;

Controller::Controller()
{
	TRACE("construct");
}

Controller::~Controller()
{
	TRACE("destruct");
}

void Controller::run()
{
	INFO("run");

	fsm::start();

	eInitialize event;
	fsm::dispatch( event );
}

void Controller::halt()
{
	INFO("halt");

	eTerminate event;
	fsm::dispatch( event );
}

/** Observer callbacks **/
void Controller::initialized()
{
	bool done = false;

	if( done )
	{
		eInitialized event;
		fsm::dispatch( event );
	}
}

void Controller::ready()
{

}

void Controller::terminated()
{
	bool done = false;

	if( done )
	{
		eTerminated event;
		fsm::dispatch( event );
	}
}

void Controller::error()
{
	eError event;
	fsm::dispatch( event );
}

#include <chrono>

#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"
#include "Controller.h"
#include "fsm/StateBase.h"

struct eInitialize : tinyfsm::Event { };

class MosquittoState:
	public tinyfsm::Fsm<MosquittoState>
{

public:
	virtual void react(tinyfsm::Event const &)
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

class sInit: 
	public MosquittoState
{

};

FSM_INITIAL_STATE( MosquittoState, sInit )

using fsm = MosquittoState;

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

	active = true;

	while( active )
	{
		TRACE("cycle");

		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
	}
}

void Controller::halt()
{
	INFO("halt");

	active = false;
}

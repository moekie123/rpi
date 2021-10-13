
#include <memory>
#include <tinyfsm/tinyfsm.hpp>

#include "Logger.h"
#include "MqttClient.h"

struct eCycle : tinyfsm::Event { };

struct sIdle;

class MqttClientState:
	public tinyfsm::Fsm<MqttClientState>
{

public:
	virtual void react( eCycle const & ){}

	virtual void react( tinyfsm::Event const & )
	{
		DEBUG("mqtt react");
	};

	virtual void entry(void)
	{
		DEBUG("mqtt entry");
	};

  	virtual void exit(void)
	{
		DEBUG("mqtt exit");
	};

	static void attach( Observer* observer )
	{
		observers.push_back( observer );
	}

protected:
	inline static std::vector< Observer* > observers;

};

class sIdle: 
	public MqttClientState
{
	using base = MqttClientState;

};

FSM_INITIAL_STATE( MqttClientState, sIdle )

MqttClient::MqttClient()
{
	TRACE("construct");
}

MqttClient::~MqttClient()
{
	TRACE("destroy");
}

void MqttClient::attach( Observer* observer )
{
	DEBUG("attach");
	MqttClientState::attach( observer );
}


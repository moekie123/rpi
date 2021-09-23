#include <vector>
#include <cerrno>

#include <mosquitto.h>
#include <spdlog/spdlog.h>

#include "Channel.h"

struct InitializeState;
struct ConfigureState;
struct ControlState;
struct TerminateState;

void Channel::react( HaltEvent const &) 
{ 
	spdlog::info("Halt");
	halt = true;
};

void Channel::entry() 
{ 
	spdlog::trace("Entry");
	cntr = 0;
};

void Channel::exit()
{   	
	spdlog::trace("Exit");
};

/**
 * State: Initialize
 **/
struct InitializeState : Channel
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

		if( this->initialize() )
			transit<ConfigureState>();

	};
};

bool Channel::initialize() 
{
	int nChannels;

	spdlog::info("init filesystem");

	spdlog::info("init channel {}", iChannel);

	const std::vector<std::string> fsChip = { "", "npwm", "export", "unexport"};

	for ( auto file : fsChip) 
	{
		const std::string dFile = dChip + file;

		spdlog::debug("chip {}", dFile );
		if( !std::filesystem::exists( dFile ) )
		{
			spdlog::error("Failed to find {}", dFile );
			return false;
		}
	}
	
	std::ifstream fsNpwm( dChip + "npwm" );
	fsNpwm >> nChannels;
	fsNpwm.close();

	spdlog::debug("channels {}", nChannels);

	if( iChannel > nChannels )
		return false;

	this->enable();
	
	spdlog::info("channel {}", dChannel);

	const std::vector<std::string> fsChannel = { "", "duty_cycle", "enable", "period",  "polarity" };

	for ( auto file : fsChannel) 
	{
		spdlog::debug("channel io {}", dChannel + file );
		if( !std::filesystem::exists( dChannel + file ) )
		{
			spdlog::error("failed to find {}", dChannel + file );
			return -1;
		}
	}

	return true;
}

/**
 * State: Configure
 **/
struct ConfigureState: Channel
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Configure {}", cntr);
	cntr += 1;

	if( cntr >= 3 || halt )
	{
		transit<InitializeState>();
		return;
	}

	if( this->configure() )
		transit<ControlState>();
  };
};

bool Channel::configure() 
{
	spdlog::info("configure");

	this->setPeriod( 20000000 );
	this->setDutyCycle( dutycycle );
	this->enable();

	return true;
}

/**
 * State: Control
 **/
struct ControlState : Channel
{
  	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Publish {}", cntr);
		cntr += 1;

		if( halt )
		{
			transit<TerminateState>();
			return;
		}

		if( this->control() )
		{

		}	
		else
			transit<ConfigureState>();
	};
};

bool Channel::control() 
{
	spdlog::info("control");

	dutycycle += 100000;
	this->setDutyCycle( dutycycle );

	return true;
}

/**
 * State: Terminate
 **/
struct TerminateState: Channel
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

	if( this->terminate() )
		transit<Idle>();
  };
};

bool Channel::terminate() 
{
	spdlog::info("terminate");

	this->disable();

	return true;
}

void Channel::enable()
{
	const std::string dExport = dChip + "export";
	
	spdlog::debug("{}::{}", dExport, iChannel);

	std::ofstream fsExport( dExport );
	fsExport << iChannel << '\n';
	fsExport.close();

	const std::string dEnable = dChannel + "enable";
	
	spdlog::debug("{}::1", dEnable);

	std::ofstream fsEnable( dEnable );
	fsEnable << 1 << '\n';
	fsEnable.close();
}

void Channel::disable()
{
	const std::string dEnable = dChannel + "enable";

	spdlog::debug("{}::0", dEnable);

	std::ofstream fsEnable( dEnable );
	fsEnable << 0 << '\n';
	fsEnable.close();

	const std::string dUnexport = dChip + "unexport";
	
	spdlog::debug("{}::{}", dUnexport, iChannel);

	std::ofstream fsUnexport( dUnexport );
	fsUnexport << iChannel << '\n';
	fsUnexport.close();
}

void Channel::setPeriod( const long period )
{
	const std::string dPeriod = dChannel + "period";

	spdlog::debug("{}::{}", dPeriod, period);

	std::ofstream fsPeriod( dPeriod );
	fsPeriod << period << '\n';
	fsPeriod.close();
}

void Channel::setDutyCycle( const long dutycycle )
{
	const std::string dDutyCycle = dChannel + "duty_cycle";

	spdlog::debug("{}::{}", dDutyCycle, dutycycle);

	std::ofstream fsDutyCycle( dDutyCycle );
	fsDutyCycle << dutycycle << '\n';
	fsDutyCycle.close();
}

FSM_INITIAL_STATE( Channel, InitializeState )

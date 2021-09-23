#include <vector>
#include <cerrno>

#include <mosquitto.h>
#include <spdlog/spdlog.h>

#include "Channel.h"

template<int,int>
struct InitializeState;

template<int,int>
struct ConfigureState;

template<int,int>
struct ControlState;

template<int,int>
struct TerminateState;

template<int iChip, int iChannel>
void Channel<iChip,iChannel>::react( HaltEvent const &) 
{ 
	using base = Channel<iChip,iChannel>;

	spdlog::info("Halt");
	base::halt = true;
};

template<int iChip, int iChannel>
void Channel<iChip,iChannel>::entry() 
{
	using base = Channel<iChip,iChannel>;

	spdlog::trace("Entry");
	base::cntr = 0;
};

template<int iChip, int iChannel>
void Channel<iChip,iChannel>::exit()
{   	
	spdlog::trace("Exit");
};

/**
 * State: Initialize
 **/
template<int iChip, int iChannel>
struct InitializeState : Channel<iChip,iChannel>
{
	using base = Channel<iChip,iChannel>;

	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Initialize {}", base::cntr);
		base::cntr += 1;

		if( base::cntr >= 3 || base::halt )
		{
			base::template transit<Idle<iChip,iChannel>>();
			return;
		}

		if( this->initialize() )
			base::template transit<ConfigureState<iChip,iChannel>>();

	};
};

template<int iChip, int iChannel>
bool Channel<iChip,iChannel>::initialize() 
{
	using base = Channel<iChip,iChannel>;

	int nChannels;

	spdlog::info("init filesystem");

	spdlog::info("init channel {}", iChannel);

	const std::vector<std::string> fsChip = { "", "npwm", "export", "unexport"};

	for ( auto file : fsChip) 
	{
		const std::string dFile = base::dChip + file;

		spdlog::debug("chip {}", dFile );
		if( !std::filesystem::exists( dFile ) )
		{
			spdlog::error("Failed to find {}", dFile );
			return false;
		}
	}
	
	std::ifstream fsNpwm( base::dChip + "npwm" );
	fsNpwm >> nChannels;
	fsNpwm.close();

	spdlog::debug("channels {}", nChannels);

	if( iChannel > nChannels )
		return false;

	this->enable();
	
	spdlog::info("channel {}", base::dChannel);

	const std::vector<std::string> fsChannel = { "", "duty_cycle", "enable", "period",  "polarity" };

	for ( auto file : fsChannel) 
	{
		spdlog::debug("channel io {}", base::dChannel + file );
		if( !std::filesystem::exists( base::dChannel + file ) )
		{
			spdlog::error("failed to find {}", base::dChannel + file );
			return -1;
		}
	}

	return true;
}

/**
 * State: Configure
 **/
template<int iChip, int iChannel>
struct ConfigureState: Channel<iChip,iChannel>
{
	using base = Channel<iChip,iChannel>;

  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Configure {}", base::cntr);
	base::cntr += 1;

	if( base::cntr >= 3 || base::halt )
	{
		base::template transit<InitializeState<iChip,iChannel>>();
		return;
	}

	if( this->configure() )
		base::template transit<ControlState<iChip,iChannel>>();
  };
};

template<int iChip, int iChannel>
bool Channel<iChip,iChannel>::configure() 
{
	using base = Channel<iChip,iChannel>;

	spdlog::info("configure");

	this->setPeriod( 20000000 );
	this->setDutyCycle( dutycycle );
	this->enable();

	return true;
}

/**
 * State: Control
 **/
template<int iChip, int iChannel>
struct ControlState : Channel<iChip,iChannel>
{
	using base = Channel<iChip,iChannel>;

  	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Publish {}", base::cntr);
		base::cntr += 1;

		if( base::halt )
		{
			base::template transit<TerminateState<iChip,iChannel>>();
			return;
		}

		if( this->control() )
		{

		}	
		else
			base::template transit<ConfigureState<iChip,iChannel>>();
	};
};

template<int iChip, int iChannel>
bool Channel<iChip,iChannel>::control() 
{
	using base = Channel<iChip,iChannel>;

	spdlog::info("control");

	dutycycle += 100000;
	this->setDutyCycle( dutycycle );

	return true;
}

/**
 * State: Terminate
 **/
template<int iChip, int iChannel>
struct TerminateState: Channel<iChip,iChannel>
{
	using base = Channel<iChip,iChannel>;

  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Terminate {}", base::cntr);
	base::cntr += 1;

	if( base::cntr > 3 )
	{
		base::template transit<Idle<iChip,iChannel>>();
		return;
	}

	if( this->terminate() )
		base::template transit<Idle<iChip,iChannel>>();
  };
};

template<int iChip, int iChannel>
bool Channel<iChip,iChannel>::terminate() 
{
	using base = Channel<iChip,iChannel>;

	spdlog::info("terminate");

	this->disable();

	return true;
}

template<int iChip, int iChannel>
void Channel<iChip,iChannel>::enable()
{
	using base = Channel<iChip,iChannel>;

	const std::string dExport = base::dChip + "export";
	
	spdlog::debug("{}::{}", dExport, iChannel);

	std::ofstream fsExport( dExport );
	fsExport << iChannel << '\n';
	fsExport.close();

	const std::string dEnable = base::dChannel + "enable";
	
	spdlog::debug("{}::1", dEnable);

	std::ofstream fsEnable( dEnable );
	fsEnable << 1 << '\n';
	fsEnable.close();
}

template<int iChip, int iChannel>
void Channel<iChip,iChannel>::disable()
{
	using base = Channel<iChip,iChannel>;

	const std::string dEnable = base::dChannel + "enable";

	spdlog::debug("{}::0", dEnable);

	std::ofstream fsEnable( dEnable );
	fsEnable << 0 << '\n';
	fsEnable.close();

	const std::string dUnexport = base::dChip + "unexport";
	
	spdlog::debug("{}::{}", dUnexport, iChannel);

	std::ofstream fsUnexport( dUnexport );
	fsUnexport << iChannel << '\n';
	fsUnexport.close();
}

template<int iChip, int iChannel>
void Channel<iChip,iChannel>::setPeriod( const long period )
{
	using base = Channel<iChip,iChannel>;

	const std::string dPeriod = base::dChannel + "period";

	spdlog::debug("{}::{}", dPeriod, period);

	std::ofstream fsPeriod( dPeriod );
	fsPeriod << period << '\n';
	fsPeriod.close();
}

template<int iChip, int iChannel>
void Channel<iChip,iChannel>::setDutyCycle( const long dutycycle )
{
	using base = Channel<iChip,iChannel>;

	const std::string dDutyCycle = base::dChannel + "duty_cycle";

	spdlog::debug("{}::{}", dDutyCycle, dutycycle);

	std::ofstream fsDutyCycle( dDutyCycle );
	fsDutyCycle << dutycycle << '\n';
	fsDutyCycle.close();
}

#define COMMA ,
FSM_INITIAL_STATE( Channel<0 COMMA 14>, InitializeState<0 COMMA 14> )
FSM_INITIAL_STATE( Channel<0 COMMA 15>, InitializeState<0 COMMA 15> )

#include <vector>
#include <cerrno>

#include <mosquitto.h>
#include <spdlog/spdlog.h>

#include "Device.h"

template<int,int>
struct InitializeState;

template<int,int>
struct ConfigureState;

template<int,int>
struct ControlState;

template<int,int>
struct TerminateState;

template<int iChip, int iChannel>
void Device<iChip,iChannel>::react( HaltEvent const &) 
{ 
	using base = Device<iChip,iChannel>;

	spdlog::info("Halt");
	base::halt = true;
};

template<int iChip, int iChannel>
void Device<iChip,iChannel>::entry() 
{
	using base = Device<iChip,iChannel>;

	spdlog::trace("Entry");
	base::cntr = 0;
};

template<int iChip, int iChannel>
void Device<iChip,iChannel>::exit()
{   	
	spdlog::trace("Exit");
};

/**
 * State: Initialize
 **/
template<int iChip, int iChannel>
struct InitializeState : Device<iChip,iChannel>
{
	using base = Device<iChip,iChannel>;

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
bool Device<iChip,iChannel>::initialize() 
{
	using base = Device<iChip,iChannel>;

	int nChannels;

	spdlog::info("init filesystem");

	spdlog::info("init channel {}", iChannel);

	const std::vector<std::string> fsChip = { "", "npwm", "export", "unexport"};

	for ( auto file : fsChip) 
	{
		const std::string dFile = base::fChip + file;

		spdlog::debug("chip {}", dFile );
		if( !std::filesystem::exists( dFile ) )
		{
			spdlog::error("Failed to find {}", dFile );
			return false;
		}
	}
	
	std::ifstream fsNpwm( base::fChip + "npwm" );
	fsNpwm >> nChannels;
	fsNpwm.close();

	spdlog::debug("channels {}", nChannels);

	if( iChannel > nChannels )
		return false;

	this->enable();
	
	spdlog::info("channel {}", base::fChannel);

	const std::vector<std::string> fsChannel = { "", "duty_cycle", "enable", "period" };

	for ( auto file : fsChannel) 
	{
		spdlog::debug("channel io {}", base::fChannel + file );
		if( !std::filesystem::exists( base::fChannel + file ) )
		{
			spdlog::error("failed to find {}", base::fChannel + file );
			return -1;
		}
	}

	return true;
}

/**
 * State: Configure
 **/
template<int iChip, int iChannel>
struct ConfigureState: Device<iChip,iChannel>
{
	using base = Device<iChip,iChannel>;

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
bool Device<iChip,iChannel>::configure() 
{
	using base = Device<iChip,iChannel>;

	spdlog::info("configure");

	this->setPeriod( this->period );
	this->setDutyCycle( this->dutycycle );
	this->enable();

	return true;
}

/**
 * State: Control
 **/
template<int iChip, int iChannel>
struct ControlState : Device<iChip,iChannel>
{
	using base = Device<iChip,iChannel>;

  	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Control {}", base::cntr);
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
bool Device<iChip,iChannel>::control() 
{
	using base = Device<iChip,iChannel>;

	spdlog::info("control");

	this->dutycycle += 100000;
	this->setDutyCycle( this->dutycycle );

	return true;
}

/**
 * State: Terminate
 **/
template<int iChip, int iChannel>
struct TerminateState: Device<iChip,iChannel>
{
	using base = Device<iChip,iChannel>;

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
bool Device<iChip,iChannel>::terminate() 
{
	using base = Device<iChip,iChannel>;

	spdlog::info("terminate");

	this->disable();

	return true;
}

template<int iChip, int iChannel>
void Device<iChip,iChannel>::enable()
{
	using base = Device<iChip,iChannel>;

	const std::string fExport = base::fChip + "export";
	
	spdlog::debug("{}::{}", fExport, iChannel);

	std::ofstream fsExport( fExport );
	fsExport << iChannel << '\n';
	fsExport.close();

	const std::string fEnable = base::fChannel + "enable";
	
	spdlog::debug("{}::1", fEnable);

	std::ofstream fsEnable( fEnable );
	fsEnable << 1 << '\n';
	fsEnable.close();
}

template<int iChip, int iChannel>
void Device<iChip,iChannel>::disable()
{
	using base = Device<iChip,iChannel>;

	const std::string fEnable = base::fChannel + "enable";

	spdlog::debug("{}::0", fEnable);

	std::ofstream fsEnable( fEnable );
	fsEnable << 0 << '\n';
	fsEnable.close();

	const std::string fUnexport = base::fChip + "unexport";
	
	spdlog::debug("{}::{}", fUnexport, iChannel);

	std::ofstream fsUnexport( fUnexport );
	fsUnexport << iChannel << '\n';
	fsUnexport.close();
}

template<int iChip, int iChannel>
void Device<iChip,iChannel>::setPeriod( const long period )
{
	using base = Device<iChip,iChannel>;

	const std::string fPeriod = base::fChannel + "period";

	spdlog::debug("{}::{}", fPeriod, period);

	std::ofstream fsPeriod( fPeriod );
	fsPeriod << period << '\n';
	fsPeriod.close();
}

template<int iChip, int iChannel>
void Device<iChip,iChannel>::setDutyCycle( const long dutycycle )
{
	using base = Device<iChip,iChannel>;

	const std::string fDutyCycle = base::fChannel + "duty_cycle";

	spdlog::debug("{}::{}", fDutyCycle, dutycycle);

	std::ofstream fsDutyCycle( fDutyCycle );
	fsDutyCycle << dutycycle << '\n';
	fsDutyCycle.close();
}

#define COMMA ,
FSM_INITIAL_STATE( Device<0 COMMA 14>, InitializeState<0 COMMA 14> )
FSM_INITIAL_STATE( Device<0 COMMA 15>, InitializeState<0 COMMA 15> )

#pragma once

#include <filesystem>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "Observer.h"
#include "StateMachine.h"

/* --------------------------------------------------------------------------*/
template<typename... FF>
struct DeviceList;

template<> struct DeviceList<> {
	static void attach( Observer* ) { }
};

template<typename F, typename... FF>
struct DeviceList<F, FF...>: tinyfsm::FsmList<F, FF...>
{
	static void attach( Observer* observer ) 
	{
		F::attach(observer);
		DeviceList<FF...>::attach(observer);
	}
};

/* --------------------------------------------------------------------------*/

template<int iChip, int iChannel>
struct Device : tinyfsm::Fsm<Device<iChip,iChannel>>
{
	public:
		inline static bool halt = false;

		virtual void entry();
		virtual void exit();

		virtual void react( CycleEvent const &) = 0;
		virtual void react( HaltEvent const &);

		virtual bool initialize();
		virtual bool configure();
		virtual bool terminate();
		virtual bool control();

		/* Observer Pattern */
		static void attach( Observer* );

	protected:
		Device( const std::string _name ): name(_name){};

		int cntr;
		const std::string name;

		inline static long period = 2000000;
		inline static long dutycycle = 1000000;

		inline static const int chipID = iChip;
		inline static const int channelID = iChannel;
		inline static const std::string fSystemRoot = "/sys/class/pwm/";
		inline static const std::string fChip = fSystemRoot + "pwmchip" + std::to_string( chipID ) + "/";
		inline static const std::string fChannel = fSystemRoot + "pwm" + std::to_string( channelID ) + "/";

	private:
		void enable();
		void disable();

		void setPeriod( long );
		void setDutyCycle( long );

		/* Observer Pattern */
		inline static std::vector< Observer* > observers;
};

template<int iChip, int iChannel>
struct Idle : Device<iChip,iChannel>
{
	using base = Device<iChip,iChannel>;

	Idle(): Device<iChip,iChannel>("idle"){}

	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Idle {}", base::cntr);
		base::cntr += 1;

		if( base::halt ) return;
  	};
};

template<int iChip, int iChannel>
void Device<iChip,iChannel>::attach( Observer* observer )
{   	
	spdlog::trace("Attach");
	observers.push_back( observer );
};


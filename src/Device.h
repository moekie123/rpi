#pragma once

#include <filesystem>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "StateMachine.h"

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

	protected:
		int cntr;

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
};

template<int iChip, int iChannel>
struct Idle : Device<iChip,iChannel>
{
	using base = Device<iChip,iChannel>;

	void react( CycleEvent const & ) override 
	{
		spdlog::trace("Idle {}", base::cntr);
		base::cntr += 1;

		if( base::halt ) return;
  	};
};


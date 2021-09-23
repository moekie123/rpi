#pragma once

#include <filesystem>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include "StateMachine.h"

struct Channel : tinyfsm::Fsm<Channel>
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

		inline static long dutycycle = 1000000;

		inline static const int iChip = 0;
		inline static const int iChannel = 15;
		inline static const std::string dRoot = "/sys/class/pwm/";
		inline static const std::string dChip = dRoot + "pwmchip" + std::to_string( iChip ) + "/";
		inline static const std::string dChannel = dRoot + "pwm" + std::to_string( iChannel ) + "/";

	private:
		void enable();
		void disable();

		void setPeriod( long );
		void setDutyCycle( long );
};

struct Idle : Channel
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Idle {}", cntr);
	cntr += 1;

	if( halt ) return;
  };
};


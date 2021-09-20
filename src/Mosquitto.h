#pragma once

#include <spdlog/spdlog.h>
#include <tinyfsm/tinyfsm.hpp>

#include <mosquitto.h>

struct CycleEvent: tinyfsm::Event{};
struct HaltEvent: tinyfsm::Event{};

struct Mosquitto : tinyfsm::Fsm<Mosquitto>
{
	public:
		inline static bool halt = false;

		virtual void entry();
		virtual void exit();

		virtual void react( CycleEvent const &) = 0;
		virtual void react( HaltEvent const &);

		virtual bool init_mosquitto();

		virtual bool configure();
		virtual bool connect();
		virtual bool disconnect();
		virtual bool terminate();
		virtual bool publish();

	protected:
		int cntr;

	private:
		inline static mosquitto* client = NULL;
};

struct Idle : Mosquitto
{
  void react( CycleEvent const & ) override 
  {
	spdlog::trace("Idle {}", cntr);
	cntr += 1;

	if( halt ) return;
  };
};



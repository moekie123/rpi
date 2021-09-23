#pragma once

#include <tinyfsm/tinyfsm.hpp>

struct CycleEvent: tinyfsm::Event{};
struct HaltEvent: tinyfsm::Event{};


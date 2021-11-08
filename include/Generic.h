#pragma once

#include "Logger.h"

class Generic {

  public:
    const std::string &getName() { return name; }

  protected:
    std::string name;
};

#pragma once

#include "Logger.h"

//! Interface base class
/*!
 * An fundamental base class for all Interface classes
 */
class Generic {

  public:
    //! (Getter) Name
    const std::string &getName() { return name; }

  protected:
    //! The name of the object
    std::string name;
};

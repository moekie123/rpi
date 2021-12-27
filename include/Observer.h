#pragma once

#include <string>

#include "Generic.h"

//! Interface base class
/*!
 * Observerable/Observer design pattern
 * This base class can be attached to an observable through the
 * Observable::attach()
 */
class Observer : public virtual Generic {
  public:
    virtual ~Observer(){};

    //! Observer callback
    /*!
     * Object that inherent an Observable can call Observable::notify() to
     * invoke this update \param cmd An tag representing the type of the update
     * \param data Additional data attached by the update
     */
    virtual void update( const std::string &cmd, const void *data ) = 0;
};

#pragma once

#include "Generic.h"

//! Interface base class
/*!
 * 
 */
class Runnable : public virtual Generic {

  public:
	
	//! Invoke to start processing logic of the object
    virtual void start() = 0;

	//! Invoke to stop processing logic of the object
    virtual void stop() = 0;

	//! Request the state of the object
	/*!
	 *	\return True, object is running
	 */	
    virtual bool isRunning() { return running; }

  protected:
	//! State of the object, True = active
    bool running = false;
};

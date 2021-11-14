#pragma once

#include <vector>

#include "Logger.h"

#include "Generic.h"
#include "Observer.h"

//! Interface base class
/*!
 * Observerable/Observer design pattern
 */
class Observable : public virtual Generic {

  public:
	
	//! Attacher of observers
	/*!
	 * \param observer The observer that requests to be informed
	 */
    void attach( Observer *observer )
    {
        logger::trace( this->name + ": attach observer: {}",
                       fmt::ptr( observer ) );
        observers.push_back( observer );
    }

	//! Observers notifier
	/*!
	 * Notify shall iterate through all observers and call it's update()
	 * \param cmd An tag representing the type of update
	 * \param data Additional data attached by the update
	 */
    void notify( const std::string &cmd, const void *data )
    {
        logger::trace( this->name + ": Observable: {}", cmd );

        for ( auto observer : observers )
            observer->update( cmd, data );
    }

  private:
    std::vector< Observer * > observers;
};

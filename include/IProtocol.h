#pragma once

#include "Observable.h"

//! Protocol Interface
class IProtocol : public virtual Generic, 
				  public Observable {
  public:

	//! Configure the protocol
	/*!
	 * Configure the protocol in order to enstablish a connection
	 */
    virtual void configure() = 0;

	//! Connect the protocol
	/*!
	 * Enstablish an connection
	 */
    virtual void connect() = 0;

	//! Disconnect the protocol
	/*!
	 * Close the connection
	 */
    virtual void disconnect() = 0;

	//! Destroy the protocol
	/*!
	 * Cleanup the protocol 
	 */
    virtual void destroy() = 0;
};

#pragma once

#include <stdexcept>
#include <string>
#include <tinyfsm/tinyfsm.hpp>

#include "Observable.h"
#include "Observer.h"
#include "Runnable.h"

#include "IProtocol.h"

/* States */
struct sUninitialized;
struct sConnecting;
struct sIdle;
struct sDisconnecting;
struct sTerminated;
struct sError;

/* Events */
struct eStartup : tinyfsm::Event {
};
struct eTerminate : tinyfsm::Event {
};
struct eConnected : tinyfsm::Event {
};
struct eDisconnected : tinyfsm::Event {
};
struct eRetry : tinyfsm::Event {
};

//!  Client Base State
/*!
        All states in the FSM should have this ClientState as base class.
*/
class ClientState : public tinyfsm::Fsm< ClientState >, public Observable {

  public:
    //! Base constructor
    /*!
            \param name The name of the state, without the path
    */
    ClientState( const std::string name ) : name( name ) {}

    /*!
     * This should contain state specific logic when entering the state. 
	 * It is invoked by the entry() method and should be overriden in the derived class.
     */
    virtual inline void entering(){};

    //! The 'entering state' callback for the FSM
    /*!
     * This method functions as a wrapper around entering().
	 * When entering a state the 'retry counter' will be reset, and the entering() method will be invoked.
     */
    void entry( void )
    {
        logger::trace( prefix + "/" + name + ": entry" );

        retryCntr = 0;

        entering();
    }

    /*!
     * This should contain state specific logic when exiting the state.
 	 * It is invoked by the exit() method and should be overriden in the derived class.
   	 */
    virtual inline void exiting(){};

    //! The 'exiting state' callback for the FSM
    /*!
     * This method functions as a wrapper around exiting().
     * When exiting a state the exiting() method will be invoked.
     */
    void exit( void )
    {
        logger::trace( prefix + "/" + name + ": exit" );
        exiting();
    };

	
	//! General Event
    virtual void react( const tinyfsm::Event & )
    {
        throw std::logic_error( "not implemented" );
    };

	//! Startup Event
    virtual void react( const eStartup & )
    {
        throw std::logic_error( "not implemented" );
    };

	//! Connected Event
    virtual void react( const eConnected & )
    {
        throw std::logic_error( "not implemented" );
    };

	//! Disconnected Event
    virtual void react( const eDisconnected & )
    {
        throw std::logic_error( "not implemented" );
    };

	//! This should contain the state specific logic when retrying the state
    virtual inline void retry(){};

 	//! The 'retrying event' callback for the FSM
	/*!
	 *	This method funtions as a wrapper around retry()
	 *	When retry event is triggerd the retry() mehtod is invoked
	 */	
	void react( const eRetry & )
    {
        logger::trace( prefix + "/" + name + ": retry [{}/{}]", retryCntr,
                       retryMax );

        if ( ( retryCntr > retryMax ) )
            ClientState::transit< sError >();
        else
            retry();

        retryCntr++;
    }

	//! This should contain the state specific logic when terminate event is triggered
    virtual inline void terminate(){};
 	
	//! The 'terminate event' callback for the FSM
	/*!
	 * This method functions as a wrapper around terminate()
	 * When the terminate event is triggered the terminate() method is invoked
	 */	
 	void react( const eTerminate & )
    {
        logger::trace( prefix + "/" + name + ": eTerminate" );
        terminate();
    };

	//! Visitor pattern: Attach the protocol
    static void accept( IProtocol *protocol )
    {
        logger::trace( prefix + ": accept protocol: {}", fmt::ptr( protocol ) );

        ClientState::protocol = protocol;
    }

    /*!
     * The absolute path of the object
     */
    static inline std::string prefix = "/";

  protected:
    /*!
     * The name of the state
     */
    const std::string name;

    /*!
     * The communication protocol through which the client statemachine shall
     * exchange data
     */
    static inline IProtocol *protocol = nullptr;

  private:
    int retryMax = 3;
    int retryCntr;
};

//! Concrete State: Uninitialized
class sUninitialized : public ClientState {
  public:
    sUninitialized() : ClientState( "uninitialized" ) {}

    void react( const eStartup & ) override
    {
        logger::trace( prefix + "/" + name + ": sUninitialized::eStartup" );
        protocol->configure();

        ClientState::transit< sConnecting >();
    };
};

//! Concrete State: Connecting
class sConnecting : public ClientState {
  public:
    sConnecting() : ClientState( "connecting" ) {}

    void react( const eConnected & ) override
    {
        ClientState::transit< sIdle >();
    }

  private:
    inline void entering() override { protocol->connect(); };

    void retry() override { protocol->connect(); }

    inline void terminate() override { ClientState::transit< sTerminated >(); };
};

//! Concrete State: Idle
class sIdle : public ClientState {
  public:
    sIdle() : ClientState( "idle" ) {}

  private:
    inline void terminate() override
    {
        ClientState::transit< sDisconnecting >();
    };
};

//! Concrete State: Disconnecting
class sDisconnecting : public ClientState {
  public:
    sDisconnecting() : ClientState( "disconnecting" ) {}

    void react( const eDisconnected & ) override
    {
        ClientState::transit< sTerminated >();
    }

  private:
    inline void entering() override { protocol->disconnect(); };

    void retry() override { protocol->disconnect(); }
};

//! Concrete State: Terminated
class sTerminated : public ClientState {
  public:
    sTerminated() : ClientState( "terminated" ) {}

  private:
    inline void entering() override { protocol->destroy(); }
};

//! Concrete State: Error
class sError : public ClientState {
  public:
    sError() : ClientState( "error" ) {}

  private:
    inline void terminate() override { ClientState::transit< sTerminated >(); };
};

FSM_INITIAL_STATE( ClientState, sUninitialized )

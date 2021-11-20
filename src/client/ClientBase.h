#pragma once

#include <string>

#include "IClient.h"

#include "Observable.h"
#include "Observer.h"

//! Base class for all Clients
class ClientBase : public IClient {
  public:
    /*!
     * \param name
     * \param protocol
     */
    ClientBase( const std::string name, IProtocol *protocol );

    ~ClientBase();

    /* Client Interface */
    void start() override;
    void stop() override;

    /* Observer */
    void update( const std::string &cmd, const void *data ) override;
};


#pragma once

#include <string>

#include "IClient.h"
#include "IController.h"

//! Base class for controllers
class ControllerBase : public IController {
  public:
    //! Constructor
    ControllerBase( const std::string, IClient * );
    ~ControllerBase();

    /* Controller Interface */
    void start() override;
    void stop() override;

    /* Observer */
    void update( const std::string &, const void * ) override;

  private:
    IClient *client;
};

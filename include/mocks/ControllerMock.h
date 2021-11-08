#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "IController.h"

class ControllerMock : public IController {
    // Generic
    MOCK_METHOD( const std::string &, getName, () );

    // Runnable
    MOCK_METHOD( void, start, (), ( override ) );
    MOCK_METHOD( void, stop, (), ( override ) );
    MOCK_METHOD( bool, isRunning, (), ( override ) );

    // Observable
    MOCK_METHOD( void, attach, (Observer *));
    MOCK_METHOD( void, notify, (const std::string &, const void *));

    // Observer
    MOCK_METHOD( void, update, (const std::string &, const void *),
                 ( override ) );
};

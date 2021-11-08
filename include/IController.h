#pragma once

#include "Generic.h"
#include "Runnable.h"

#include "Observable.h"
#include "Observer.h"

class IController : public virtual Generic,
                    public Runnable,
                    public Observable,
                    public Observer {
};

#pragma once
#include "project.h"

#include "core_time/CommonClock.h"

namespace RF { namespace cc { namespace input {
///////////////////////////////////////////////////////////////////////////////

// TODO: This is obviously gross, replace it
void HardcodedSetup();
void HardcodedTick();
void HardcodedAdvance( time::CommonClock::time_point lockedFrame, time::CommonClock::time_point newWriteHead );

///////////////////////////////////////////////////////////////////////////////
}}}

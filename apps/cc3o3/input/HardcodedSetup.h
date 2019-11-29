#pragma once
#include "project.h"

#include "cc3o3/input/InputFwd.h"

#include "Rollback/RollbackFwd.h"

#include "core_time/CommonClock.h"

namespace RF { namespace cc { namespace input {
///////////////////////////////////////////////////////////////////////////////

// TODO: This is obviously gross, replace it
void HardcodedSetup();
void HardcodedRawTick();
void HardcodedRollbackTick();
void HardcodedAdvance( time::CommonClock::time_point lockedFrame, time::CommonClock::time_point newWriteHead );

void DebugQueueTestInput( time::CommonClock::time_point frame, rollback::InputStreamIdentifier streamID, rollback::InputValue input );

///////////////////////////////////////////////////////////////////////////////
}}}

#pragma once
#include "project.h"

#include "cc3o3/input/InputFwd.h"

#include "Rollback/RollbackFwd.h"

#include "core_time/CommonClock.h"

namespace RF::cc::input {
///////////////////////////////////////////////////////////////////////////////

// TODO: This is obviously gross, replace it
void HardcodedRawSetup();
void HardcodedMainSetup();
void HardcodedGameSetup();
void HardcodedHackSetup();
void HardcodedRawTick();
void HardcodedRollbackTick();
void HardcodedAdvance( time::CommonClock::time_point lockedFrame, time::CommonClock::time_point newWriteHead );
PlayerID HardcodedGetLocalPlayer();

void DebugQueueTestInput( time::CommonClock::time_point frame, rollback::InputStreamIdentifier streamID, rollback::InputValue input );
void DebugSubmitTestInput();
void DebugClearTestInput();

///////////////////////////////////////////////////////////////////////////////
}

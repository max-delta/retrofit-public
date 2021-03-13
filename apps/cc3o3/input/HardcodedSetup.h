#pragma once
#include "project.h"

#include "cc3o3/input/InputFwd.h"

namespace RF::cc::input {
///////////////////////////////////////////////////////////////////////////////

// TODO: This is obviously gross, replace it
void HardcodedRawSetup();
void HardcodedMainSetup();
void HardcodedPlayerSetup( PlayerID playerID );
void HardcodedHackSetup();
void HardcodedRawTick();
PlayerID HardcodedGetLocalPlayer();

///////////////////////////////////////////////////////////////////////////////
}

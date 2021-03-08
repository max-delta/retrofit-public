#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"


namespace RF::cc {
///////////////////////////////////////////////////////////////////////////////

enum class SimulationMode : uint8_t
{
	Invalid = 0,
	OnRailsReplay,
	SingleFrameSimulate,
	RollbackAndSimulate,
	StallSimulation
};

///////////////////////////////////////////////////////////////////////////////

void Startup();
void ProcessFrame();
void Shutdown();

SimulationMode DebugGetPreviousFrameSimulationMode();
void DebugInstantReplay( size_t numFrames );

// WARNING: Rollback behavior unreliable across global state changes
void RequestGlobalDeferredStateChange( appstate::AppStateID stateID );

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"


namespace RF { namespace cc {
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

// WARNING: Rollback behavior unreliable across global state changes
void RequestGlobalDeferredStateChange( appstate::AppStateID stateID );

///////////////////////////////////////////////////////////////////////////////
}}

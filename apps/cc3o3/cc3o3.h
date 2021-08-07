#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "rftl/string"


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
void DebugTakeSnapshot( rftl::string const& name );
void DebugLoadSnapshot( rftl::string const& name );
void DebugInstantReplay( size_t numFrames );
void DebugHardTimeReset();

// WARNING: Rollback behavior unreliable across global state changes
void RequestGlobalDeferredStateChange( appstate::AppStateID stateID );

///////////////////////////////////////////////////////////////////////////////
}

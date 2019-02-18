#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

#include "Timing/clocks.h"

#include "core/macros.h"


namespace RF { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct GAMEAPPSTATE_API AppStateTickContext
{
	RF_NO_COPY( AppStateTickContext );
	RF_NO_MOVE( AppStateTickContext );

	//
	// Stack only, created by manager
private:
	friend class AppStateManager;
	AppStateTickContext( AppStateManager& manager );


	//
	// Public data
public:
	AppStateManager& mManager;
	time::FrameClock::duration mElapsedTimeSinceLastTick = {};
	time::FrameClock::time_point mCurrentTime = {};
};

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

#include "core_time/CommonClock.h"

#include "core/macros.h"


namespace RF::appstate {
///////////////////////////////////////////////////////////////////////////////

struct GAMEAPPSTATE_API AppStateTickContext
{
	RF_NO_COPY( AppStateTickContext );
	RF_NO_MOVE( AppStateTickContext );

	//
	// Stack only, created by manager
private:
	friend class AppStateManager;
	AppStateTickContext( AppStateManager const& manager );


	//
	// Public data
public:
	AppStateManager const& mManager;
	time::CommonClock::duration mElapsedTimeSinceLastTick = {};
	time::CommonClock::time_point mCurrentTime = {};
};

///////////////////////////////////////////////////////////////////////////////
}

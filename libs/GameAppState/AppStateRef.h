#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::appstate {
///////////////////////////////////////////////////////////////////////////////

class GAMEAPPSTATE_API AppStateRef
{
	RF_NO_COPY( AppStateRef );

public:
	AppStateRef() = default;

	void OnEnter( AppStateChangeContext& context );
	void OnExit( AppStateChangeContext& context );
	void OnTick( AppStateTickContext& context );

public:
	UniquePtr<AppState> mAppState;
};

///////////////////////////////////////////////////////////////////////////////
}

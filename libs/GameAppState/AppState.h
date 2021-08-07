#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

#include "core/macros.h"


namespace RF::appstate {
///////////////////////////////////////////////////////////////////////////////

class GAMEAPPSTATE_API AppState
{
	RF_DEFAULT_COPY( AppState );

public:
	AppState() = default;
	virtual ~AppState() = default;

	virtual void OnEnter( AppStateChangeContext& context ) = 0;
	virtual void OnExit( AppStateChangeContext& context ) = 0;
	virtual void OnTick( AppStateTickContext& context ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

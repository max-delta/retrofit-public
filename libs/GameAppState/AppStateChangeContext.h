#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

#include "core/macros.h"


namespace RF { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct GAMEAPPSTATE_API AppStateChangeContext
{
	RF_NO_COPY( AppStateChangeContext );
	RF_NO_MOVE( AppStateChangeContext );

	//
	// Stack only, created by manager
private:
	friend class AppStateManager;
	AppStateChangeContext( AppStateManager const& manager );


	//
	// Public data
public:
	AppStateManager const& mManager;
};

///////////////////////////////////////////////////////////////////////////////
}}

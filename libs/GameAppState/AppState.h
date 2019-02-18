#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace appstate {
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

class GAMEAPPSTATE_API AppState
{
public:
	virtual void OnEnter( AppStateChangeContext& context ) = 0;
	virtual void OnExit( AppStateChangeContext& context ) = 0;
	virtual void OnTick( AppStateTickContext& context ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

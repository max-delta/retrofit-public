#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"


namespace RF { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

class GAMEAPPSTATE_API AppState
{
public:
	virtual ~AppState() = default;

	virtual void OnEnter( AppStateChangeContext& context ) = 0;
	virtual void OnExit( AppStateChangeContext& context ) = 0;
	virtual void OnTick( AppStateTickContext& context ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

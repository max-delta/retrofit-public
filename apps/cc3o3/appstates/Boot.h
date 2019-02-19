#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "GameAppState/AppState.h"

#include "core/macros.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

class Boot final : public AppState
{
	RF_NO_COPY( Boot );

public:
	Boot() = default;

	virtual void OnEnter( AppStateChangeContext& context ) override;
	virtual void OnExit( AppStateChangeContext& context ) override;
	virtual void OnTick( AppStateTickContext& context ) override;
};

///////////////////////////////////////////////////////////////////////////////
}}}

#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"
#include "cc3o3/ui/UIFwd.h"

#include "GameAppState/AppState.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/vector"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

class Gameplay final : public AppState
{
	RF_NO_COPY( Gameplay );

public:
	Gameplay() = default;

	virtual void OnEnter( AppStateChangeContext& context ) override;
	virtual void OnExit( AppStateChangeContext& context ) override;
	virtual void OnTick( AppStateTickContext& context ) override;


private:
	struct InternalState;
	UniquePtr<InternalState> mInternalState;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "GameAppState/AppState.h"

#include "core/ptr/unique_ptr.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

class DevTestElementLab final : public AppState
{
	RF_NO_COPY( DevTestElementLab );

public:
	DevTestElementLab() = default;

	virtual void OnEnter( AppStateChangeContext& context ) override;
	virtual void OnExit( AppStateChangeContext& context ) override;
	virtual void OnTick( AppStateTickContext& context ) override;


private:
	struct InternalState;
	UniquePtr<InternalState> mInternalState;
};

///////////////////////////////////////////////////////////////////////////////
}

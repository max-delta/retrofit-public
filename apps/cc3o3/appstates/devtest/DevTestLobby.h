#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "GameAppState/AppState.h"

#include "core/ptr/unique_ptr.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

class DevTestLobby final : public AppState
{
	RF_NO_COPY( DevTestLobby );

public:
	DevTestLobby() = default;

	virtual void OnEnter( AppStateChangeContext& context ) override;
	virtual void OnExit( AppStateChangeContext& context ) override;
	virtual void OnTick( AppStateTickContext& context ) override;


private:
	struct InternalState;
	UniquePtr<InternalState> mInternalState;
};

///////////////////////////////////////////////////////////////////////////////
}

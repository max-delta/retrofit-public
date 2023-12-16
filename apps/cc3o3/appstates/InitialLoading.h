#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "GameAppState/AppState.h"

#include "core/ptr/unique_ptr.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

class InitialLoading final : public AppState
{
	RF_NO_COPY( InitialLoading );

public:
	InitialLoading() = default;

	virtual void OnEnter( AppStateChangeContext& context ) override;
	virtual void OnExit( AppStateChangeContext& context ) override;
	virtual void OnTick( AppStateTickContext& context ) override;

	// Helpers for on-the-fly reloads during development
	static void ReloadElementDatabase();
	static void ReloadCastingEngine();


private:
	struct InternalState;
	UniquePtr<InternalState> mInternalState;
};

///////////////////////////////////////////////////////////////////////////////
}

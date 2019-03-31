#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "GameAppState/AppState.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

class TitleScreen_CharCreate final : public AppState
{
	RF_NO_COPY( TitleScreen_CharCreate );

public:
	TitleScreen_CharCreate() = default;

	virtual void OnEnter( AppStateChangeContext& context ) override;
	virtual void OnExit( AppStateChangeContext& context ) override;
	virtual void OnTick( AppStateTickContext& context ) override;


private:
	struct InternalState;
	UniquePtr<InternalState> mInternalState;
};

///////////////////////////////////////////////////////////////////////////////
}}}

#include "stdafx.h"
#include "InitialLoading.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct InitialLoading::InternalState
{
	bool mFirstFrame = true;
};

///////////////////////////////////////////////////////////////////////////////

void InitialLoading::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	// TODO: Setup UI
}



void InitialLoading::OnExit( AppStateChangeContext& context )
{
	// TODO: Wipe UI

	mInternalState = nullptr;
}



void InitialLoading::OnTick( AppStateTickContext& context )
{
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Loading..." );

	// HACK: Skip the first frame so we can get atleast 1 frame to render, then
	//  just load everything blocking
	// TODO: Asynchronous loading
	if( mInternalState->mFirstFrame )
	{
		mInternalState->mFirstFrame = false;
		return;
	}

	// TODO: Load stuff

	context.mManager.RequestDeferredStateChange( kTitleScreen );
}

///////////////////////////////////////////////////////////////////////////////
}}}

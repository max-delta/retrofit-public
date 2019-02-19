#include "stdafx.h"
#include "TitleScreen.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen::InternalState
{
	//
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	// TODO: Start sub-states
}



void TitleScreen::OnExit( AppStateChangeContext& context )
{
	// TODO: Stop sub-states

	mInternalState = nullptr;
}



void TitleScreen::OnTick( AppStateTickContext& context )
{
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Title screen" );

	// TODO: Draw background

	// TODO: Tick sub-states
}

///////////////////////////////////////////////////////////////////////////////
}}}

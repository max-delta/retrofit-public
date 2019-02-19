#include "stdafx.h"
#include "TitleScreen.h"

#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

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
	// HACK: Draw something for now to show we're in this state
	ui::Font const tempFont = app::gFontRegistry->SelectBestFont( ui::font::HalfTileSize, app::gGraphics->GetCurrentZoomFactor() );
	app::gGraphics->DrawText( gfx::PPUCoord( gfx::kTileSize * 2, gfx::kTileSize * 1 ), tempFont.mFontHeight, tempFont.mManagedFontID, "Title screen" );

	// TODO: Draw background

	// TODO: Tick sub-states
}

///////////////////////////////////////////////////////////////////////////////
}}}

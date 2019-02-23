#include "stdafx.h"
#include "TitleScreen_MainMenu.h"

#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_MainMenu::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	// TODO: ???
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_MainMenu::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	// TODO: Setup logic
	(void)internalState;

	// TODO: Setup UI
}



void TitleScreen_MainMenu::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_MainMenu::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	// HACK: Draw something for now to show we're in this state
	ui::Font const tempFont = app::gFontRegistry->SelectBestFont( ui::font::HalfTileSize, app::gGraphics->GetCurrentZoomFactor() );
	ppu.DrawText( gfx::PPUCoord( gfx::kTileSize * 2, gfx::kTileSize * 1 ), tempFont.mFontHeight, tempFont.mManagedFontID, "Title screen" );

	// TODO: Run logic
	(void)internalState;

	// TODO: Draw UI
}

///////////////////////////////////////////////////////////////////////////////
}}}

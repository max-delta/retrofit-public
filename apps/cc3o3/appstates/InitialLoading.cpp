#include "stdafx.h"
#include "InitialLoading.h"

#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"
#include "PPU/FontManager.h"

#include "PlatformFilesystem/VFS.h"

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
	app::gUiManager->RecreateRootContainer();
}



void InitialLoading::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

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

	gfx::PPUController& ppu = *app::gGraphics;

	// Load fonts
	{
		file::VFSPath const fonts = file::VFS::kRoot.GetChild( "assets", "fonts", "common" );

		// TODO: Issue load request instead of using debug access to font
		//  manager directly
		gfx::FontManager& fontMan = *ppu.DebugGetFontManager();
		gfx::ManagedFontID const narrowFont1xMono = fontMan.LoadNewResourceGetID( fonts.GetChild( "font_narrow_1x.fnt.txt" ) );
		gfx::ManagedFontID const narrowFont2xVari = fontMan.LoadNewResourceGetID( fonts.GetChild( "font_narrow_2x.fnt.txt" ) );

		ui::FontRegistry& fontReg = *app::gFontRegistry;
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont1xMono, 8, 1 } );
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont2xVari, 8, 2 } );
		fontReg.RegisterFont( ui::font::HalfTileSize, { narrowFont1xMono, gfx::kTileSize, 1 } );
		fontReg.RegisterFallbackFont( { narrowFont1xMono, 8, 1 } );
	}

	context.mManager.RequestDeferredStateChange( kTitleScreen );
}

///////////////////////////////////////////////////////////////////////////////
}}}

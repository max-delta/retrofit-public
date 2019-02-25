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
	RF_NO_COPY( InternalState );
	InternalState() = default;

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

		// TODO: Defer load requests instead of forcing immediate load
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Font, fonts.GetChild( "font_narrow_1x.fnt.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Font, fonts.GetChild( "font_narrow_2x.fnt.txt" ) );
		gfx::FontManager const& fontMan = *ppu.DebugGetFontManager();
		gfx::ManagedFontID const narrowFont1xMono = fontMan.GetManagedResourceIDFromResourceName( fonts.GetChild( "font_narrow_1x.fnt.txt" ) );
		gfx::ManagedFontID const narrowFont2xVari = fontMan.GetManagedResourceIDFromResourceName( fonts.GetChild( "font_narrow_2x.fnt.txt" ) );

		ui::FontRegistry& fontReg = *app::gFontRegistry;
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont1xMono, 8, 1 } );
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont2xVari, 8, 2 } );
		fontReg.RegisterFont( ui::font::NarrowTileMono, { narrowFont1xMono, gfx::kTileSize, 1 } );
		fontReg.RegisterFont( ui::font::LargeMenuSelection, { narrowFont2xVari, 16, 1 } );
		fontReg.RegisterFallbackFont( { narrowFont1xMono, 8, 1 } );
	}

	// Load tilesets
	// NOTE: Will cause associated textures to load
	{
		file::VFSPath const tilesets = file::VFS::kRoot.GetChild( "assets", "tilesets" );

		// TODO: Defer load requests instead of forcing immediate load
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "country_hills_back_96", tilesets.GetChild( "backgrounds", "country_hills_back_96.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "country_hills_mid_32", tilesets.GetChild( "backgrounds", "country_hills_mid_32.tset.txt" ) );
	}

	// Load framepacks
	// NOTE: Will cause associated textures to load
	{
		file::VFSPath const framepacks = file::VFS::kRoot.GetChild( "assets", "framepacks" );

		// TODO: Defer load requests instead of forcing immediate load
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, "cc303_composite_192", framepacks.GetChild( "logos", "cc303_composite_192.fpack" ) );
	}

	context.mManager.RequestDeferredStateChange( id::TitleScreen );
}

///////////////////////////////////////////////////////////////////////////////
}}}

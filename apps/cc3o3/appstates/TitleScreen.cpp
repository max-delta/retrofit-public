#include "stdafx.h"
#include "TitleScreen.h"

#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/TileLayerCSVLoader.h"

#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen::InternalState
{
	gfx::TileLayer mBackgroundBack = {};
	gfx::TileLayer mBackgroundMid = {};
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	file::VFS& vfs = *app::gVfs;

	file::VFSPath const tilemaps = file::VFS::kRoot.GetChild( "assets", "tilemaps" );

	// Setup background
	{
		internalState.mBackgroundBack = {};
		internalState.mBackgroundBack.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_back_96" );
		internalState.mBackgroundBack.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		internalState.mBackgroundBack.mXCoord = 0;
		internalState.mBackgroundBack.mYCoord = 0;
		internalState.mBackgroundBack.mZLayer = 100;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( internalState.mBackgroundBack, vfs, tilemaps.GetChild( "backgrounds", "country_hills_back.csv" ) );
			RF_ASSERT( loadSuccess );
		}

		internalState.mBackgroundMid = {};
		internalState.mBackgroundMid.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_mid_32" );
		internalState.mBackgroundMid.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		internalState.mBackgroundMid.mXCoord = 0;
		internalState.mBackgroundMid.mYCoord = 0;
		internalState.mBackgroundMid.mZLayer = 99;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( internalState.mBackgroundMid, vfs, tilemaps.GetChild( "backgrounds", "country_hills_mid.csv" ) );
			RF_ASSERT( loadSuccess );
		}
	}

	// TODO: Start sub-states
}



void TitleScreen::OnExit( AppStateChangeContext& context )
{
	// TODO: Stop sub-states

	mInternalState = nullptr;
}



void TitleScreen::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	// HACK: Draw something for now to show we're in this state
	ui::Font const tempFont = app::gFontRegistry->SelectBestFont( ui::font::HalfTileSize, app::gGraphics->GetCurrentZoomFactor() );
	ppu.DrawText( gfx::PPUCoord( gfx::kTileSize * 2, gfx::kTileSize * 1 ), tempFont.mFontHeight, tempFont.mManagedFontID, "Title screen" );

	// Draw background
	// TODO: Parallax
	internalState.mBackgroundBack.Animate();
	internalState.mBackgroundMid.Animate();
	ppu.DrawTileLayer( internalState.mBackgroundBack );
	ppu.DrawTileLayer( internalState.mBackgroundMid );

	// TODO: Tick sub-states
}

///////////////////////////////////////////////////////////////////////////////
}}}

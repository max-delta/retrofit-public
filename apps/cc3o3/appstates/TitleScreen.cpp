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
		gfx::TileLayer& back = internalState.mBackgroundBack;
		gfx::TileLayer& mid = internalState.mBackgroundMid;

		back = {};
		back.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_back_96" );
		back.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		back.mXCoord = 0;
		back.mYCoord = 0;
		back.mZLayer = 100;
		back.mWrapping = true;
		back.mLooping = true;
		back.mTimer.mMaxTimeIndex = 50;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( back, vfs, tilemaps.GetChild( "backgrounds", "country_hills_back.csv" ) );
			RF_ASSERT( loadSuccess );
		}

		mid = {};
		mid.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_mid_32" );
		mid.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		mid.mXCoord = 0;
		mid.mYCoord = 0;
		mid.mZLayer = 99;
		mid.mWrapping = true;
		mid.mLooping = true;
		mid.mTimer.mMaxTimeIndex = 10;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( mid, vfs, tilemaps.GetChild( "backgrounds", "country_hills_mid.csv" ) );
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
	{
		gfx::TileLayer& back = internalState.mBackgroundBack;
		gfx::TileLayer& mid = internalState.mBackgroundMid;

		constexpr auto parralax = [](
			gfx::PPUController const& ppu,
			gfx::TileLayer& tileLayer ) -> void
		{
			tileLayer.Animate();
			if( tileLayer.mTimer.IsFullZero() )
			{
				tileLayer.mXCoord--;
				if( tileLayer.mXCoord <= 0 )
				{
					tileLayer.mXCoord = ppu.CalculateTileLayerSize( tileLayer ).x;
				}
			}
		};
		parralax( ppu, back );
		parralax( ppu, mid );

		ppu.DrawTileLayer( back );
		ppu.DrawTileLayer( mid );
	}

	// TODO: Tick sub-states
}

///////////////////////////////////////////////////////////////////////////////
}}}

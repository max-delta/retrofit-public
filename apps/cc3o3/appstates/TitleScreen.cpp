#include "stdafx.h"
#include "TitleScreen.h"

#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "core/ptr/default_creator.h"

#include "rftl/deque"
#include "rftl/sstream"


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

	// TODO: Functionalize this, and cleanup the test in Retrofit.exe as well
	{
		gfx::ManagedTilesetID const tilesetID = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_back_96" );
		internalState.mBackgroundBack.mTilesetReference = tilesetID;
		internalState.mBackgroundBack.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		internalState.mBackgroundBack.mXCoord = 0;
		internalState.mBackgroundBack.mYCoord = 0;
		internalState.mBackgroundBack.mZLayer = 100;

		file::FileHandlePtr const tilemapHandle = vfs.GetFileForRead( tilemaps.GetChild( "backgrounds", "country_hills_back.csv" ) );
		file::FileBuffer const tilemapBuffer{ *tilemapHandle.Get(), false };
		RF_ASSERT( tilemapBuffer.GetData() != nullptr );
		{
			char const* const data = reinterpret_cast<char const*>( tilemapBuffer.GetData() );
			size_t const size = tilemapBuffer.GetSize();
			rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( data, size );

			size_t const numRows = csv.size();
			size_t longestRow = 0;
			for( rftl::deque<rftl::string> const& row : csv )
			{
				longestRow = math::Max( longestRow, row.size() );
			}

			internalState.mBackgroundBack.ClearAndResize( longestRow, numRows );

			for( size_t i_row = 0; i_row < numRows; i_row++ )
			{
				rftl::deque<rftl::string> const& row = csv.at( i_row );
				size_t const numCols = row.size();

				for( size_t i_col = 0; i_col < numCols; i_col++ )
				{
					rftl::string const& field = row.at( i_col );
					gfx::TileLayer::TileIndex val = gfx::TileLayer::kEmptyTileIndex;
					( rftl::stringstream() << field ) >> val;
					internalState.mBackgroundBack.GetMutableTile( i_col, i_row ).mIndex = val;
				}
			}
		}
	}
	{
		gfx::ManagedTilesetID const tilesetID = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_mid_32" );
		internalState.mBackgroundMid.mTilesetReference = tilesetID;
		internalState.mBackgroundMid.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		internalState.mBackgroundMid.mXCoord = 0;
		internalState.mBackgroundMid.mYCoord = 0;
		internalState.mBackgroundMid.mZLayer = 99;

		file::FileHandlePtr const tilemapHandle = vfs.GetFileForRead( tilemaps.GetChild( "backgrounds", "country_hills_mid.csv" ) );
		file::FileBuffer const tilemapBuffer{ *tilemapHandle.Get(), false };
		RF_ASSERT( tilemapBuffer.GetData() != nullptr );
		{
			char const* const data = reinterpret_cast<char const*>( tilemapBuffer.GetData() );
			size_t const size = tilemapBuffer.GetSize();
			rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( data, size );

			size_t const numRows = csv.size();
			size_t longestRow = 0;
			for( rftl::deque<rftl::string> const& row : csv )
			{
				longestRow = math::Max( longestRow, row.size() );
			}

			internalState.mBackgroundMid.ClearAndResize( longestRow, numRows );

			for( size_t i_row = 0; i_row < numRows; i_row++ )
			{
				rftl::deque<rftl::string> const& row = csv.at( i_row );
				size_t const numCols = row.size();

				for( size_t i_col = 0; i_col < numCols; i_col++ )
				{
					rftl::string const& field = row.at( i_col );
					gfx::TileLayer::TileIndex val = gfx::TileLayer::kEmptyTileIndex;
					( rftl::stringstream() << field ) >> val;
					internalState.mBackgroundMid.GetMutableTile( i_col, i_row ).mIndex = val;
				}
			}
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

#include "stdafx.h"
#include "Gameplay_Overworld.h"

#include "cc3o3/CommonPaths.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/TileLayerCSVLoader.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Overworld::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	gfx::TileLayer mTerrainLand = {};
	gfx::TileLayer mTerrainCloudA = {};
	gfx::TileLayer mTerrainCloudB = {};
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Overworld::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	file::VFS& vfs = *app::gVfs;

	// Setup terrain
	{
		gfx::TileLayer& land = internalState.mTerrainLand;
		gfx::TileLayer& cloudA = internalState.mTerrainCloudA;
		gfx::TileLayer& cloudB = internalState.mTerrainCloudB;

		land = {};
		land.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "island_mock_64" );
		land.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		land.mXCoord = 0;
		land.mYCoord = 0;
		land.mZLayer = 100;
		land.mWrapping = false;
		land.mLooping = true;
		land.mTimer.mMaxTimeIndex = 50;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( land, vfs, paths::BackgroundTilemaps().GetChild( "island_mock.csv" ) );
			RF_ASSERT( loadSuccess );
		}

		cloudA = {};
		cloudA.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "cloud_fog_a_512" );
		cloudA.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		cloudA.mXCoord = 0;
		cloudA.mYCoord = 0;
		cloudA.mZLayer = -50;
		cloudA.mWrapping = true;
		cloudA.mLooping = true;
		cloudA.mTimer.mMaxTimeIndex = 5;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( cloudA, vfs, paths::BackgroundTilemaps().GetChild( "cloud_fog_a_512.csv" ) );
			RF_ASSERT( loadSuccess );
		}

		cloudB = {};
		cloudB.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "cloud_fog_b_512" );
		cloudB.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		cloudB.mXCoord = 0;
		cloudB.mYCoord = 0;
		cloudB.mZLayer = -51;
		cloudB.mWrapping = true;
		cloudB.mLooping = true;
		cloudB.mTimer.mMaxTimeIndex = 10;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( cloudB, vfs, paths::BackgroundTilemaps().GetChild( "cloud_fog_b_512.csv" ) );
			RF_ASSERT( loadSuccess );
		}
	}
}



void Gameplay_Overworld::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Overworld::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	// Draw terrain
	{
		gfx::TileLayer& land = internalState.mTerrainLand;
		gfx::TileLayer& cloudA = internalState.mTerrainCloudA;
		gfx::TileLayer& cloudB = internalState.mTerrainCloudB;

		constexpr auto parralax =
			[](
				gfx::PPUController const& ppu,
				gfx::TileLayer& tileLayer ) -> void //
		{
			tileLayer.Animate();
			if( tileLayer.mTimer.IsFullZero() )
			{
				tileLayer.mXCoord++;
				if( tileLayer.mXCoord >= ppu.CalculateTileLayerSize( tileLayer ).x )
				{
					tileLayer.mXCoord = 0;
				}
				if( tileLayer.mXCoord % 2 == 0 )
				{
					tileLayer.mYCoord--;
					if( tileLayer.mYCoord <= 0 )
					{
						tileLayer.mYCoord = ppu.CalculateTileLayerSize( tileLayer ).y;
					}
				}
			}
		};
		parralax( ppu, cloudA );
		parralax( ppu, cloudB );

		ppu.DrawTileLayer( land );
		ppu.DrawTileLayer( cloudA );
		ppu.DrawTileLayer( cloudB );
	}

	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Overworld" );
}

///////////////////////////////////////////////////////////////////////////////
}}}

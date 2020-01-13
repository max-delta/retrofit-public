#include "stdafx.h"
#include "Gameplay_Overworld.h"

#include "cc3o3/CommonPaths.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/components/OverworldVisual.h"
#include "cc3o3/state/StateHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/TileLayerCSVLoader.h"

#include "core_state/VariableIdentifier.h"
#include "core_component/TypedObjectRef.h"

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

	// Draw overworld characters
	{
		using namespace state;

		input::PlayerID const playerID = input::HardcodedGetLocalPlayer();

		rftl::string const playerIDAsString = ( rftl::stringstream() << math::integer_cast<size_t>( playerID ) ).str();
		VariableIdentifier const companyRoot( "company", playerIDAsString );

		// Find company object
		ObjectRef const company = FindObjectByIdentifier( companyRoot );
		RFLOG_TEST_AND_FATAL( company.IsSet(), companyRoot, RFCAT_CC3O3, "Failed to find company" );

		// For each active team member...
		VariableIdentifier const rosterRoot = companyRoot.GetChild( "member" );
		comp::Roster const& roster = *company.GetComponentInstanceT<comp::Roster>();
		for( size_t i_teamIndex = 0; i_teamIndex < comp::Roster::kActiveTeamSize; i_teamIndex++ )
		{
			comp::Roster::RosterIndex const rosterIndex = roster.mActiveTeam.at( i_teamIndex );
			if( rosterIndex == comp::Roster::kInvalidRosterIndex )
			{
				continue;
			}
			rftl::string const rosterIndexAsString = ( rftl::stringstream() << math::integer_cast<size_t>( rosterIndex ) ).str();
			VariableIdentifier const charRoot = rosterRoot.GetChild( rosterIndexAsString );

			// Find character object
			MutableObjectRef const character = FindMutableObjectByIdentifier( charRoot );
			RFLOG_TEST_AND_FATAL( character.IsSet(), charRoot, RFCAT_CC3O3, "Failed to find character" );

			// HACK: Basic test drawing
			// TODO: Properly set up visuals and updating based on movement
			comp::OverworldVisual& visual = *character.GetMutableComponentInstanceT<comp::OverworldVisual>();
			visual.mObject.mZLayer = -math::integer_cast<gfx::PPUDepthLayer>( 5u - i_teamIndex );
			visual.mObject.mLooping = true;
			visual.mObject.mFramePackID = visual.mIdleSouth.mFramePackID;
			visual.mObject.mTimer.mMaxTimeIndex = visual.mIdleSouth.mMaxTimeIndex;
			visual.mObject.mTimer.mTimeSlowdown = visual.mIdleSouth.mSlowdownRate;
			visual.mObject.Animate();
			ppu.DrawObject( visual.mObject );
		}
	}

	ppu.DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Overworld" );
}

///////////////////////////////////////////////////////////////////////////////
}}}

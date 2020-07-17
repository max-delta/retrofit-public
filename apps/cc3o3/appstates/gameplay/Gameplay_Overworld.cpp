#include "stdafx.h"
#include "Gameplay_Overworld.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/components/OverworldVisual.h"
#include "cc3o3/state/components/OverworldMovement.h"
#include "cc3o3/state/StateHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/NineSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "GameInput/GameController.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/TileLayerCSVLoader.h"

#include "Timing/FrameClock.h"

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

	WeakPtr<ui::Controller> mUI;
	time::CommonClock::time_point mLastActivity;
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

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Nine-slice the whole screen
		constexpr bool kSlicesEnabled[9] = {
			false, false, true,
			false, false, false,
			true, false, false
		};
		WeakPtr<ui::controller::NineSlicer> const rootNineSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::NineSlicer>::Create(
					kSlicesEnabled ) );
		internalState.mUI = rootNineSlicer;

		// Header on top
		WeakPtr<ui::controller::Floater> const headerFloater =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize ),
					ui::Justification::TopRight ) );
		headerFloater->SetOffset( uiContext, { -gfx::kTileSize / 4, gfx::kTileSize / 4 } );
		WeakPtr<ui::controller::TextLabel> const header =
			uiManager.AssignStrongController(
				headerFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		header->SetJustification( ui::Justification::MiddleCenter );
		header->SetFont( ui::font::LargeMenuText );
		header->SetText( "UNSET" );
		header->SetColor( math::Color3f::kWhite );
		header->SetBorder( true );

		// Footer on bottom
		WeakPtr<ui::controller::Floater> const footerFloater =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 6 ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize / 2 ),
					ui::Justification::BottomLeft ) );
		footerFloater->SetOffset( uiContext, { gfx::kTileSize / 4, -gfx::kTileSize / 4 } );
		WeakPtr<ui::controller::TextLabel> const footer =
			uiManager.AssignStrongController(
				footerFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		footer->SetJustification( ui::Justification::MiddleCenter );
		footer->SetFont( ui::font::SmallMenuText );
		footer->SetText( "UNSET" );
		footer->SetColor( math::Color3f::kWhite );
		footer->SetBorder( true );

		// UI hidden unless idle for a while
		internalState.mUI->SetChildRenderingBlocked( true );
		internalState.mLastActivity = time::FrameClock::now();
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

	// Show/hide UI based on idle/activity
	time::CommonClock::duration const timeSinceLastActivity = time::FrameClock::now() - internalState.mLastActivity;
	static constexpr rftl::chrono::seconds kActivityThreshold( 3 );
	if( timeSinceLastActivity > kActivityThreshold )
	{
		internalState.mUI->SetChildRenderingBlocked( false );
	}
	else
	{
		internalState.mUI->SetChildRenderingBlocked( true );
	}

	// Process character control
	rftl::vector<input::GameCommand> const charCommands =
		InputHelpers::GetGameplayInputToProcess( input::HardcodedGetLocalPlayer(), input::layer::CharacterControl );
	for( input::GameCommand const& charCommand : charCommands )
	{
		// Treat as activity
		internalState.mLastActivity = time::FrameClock::now();

		if( charCommand.mType == input::command::game::Interact )
		{
			// HACK: Just pop into site
			// TODO: Make sure party is actually at a site, and set it up
			context.mManager.RequestDeferredStateChange( id::Gameplay_Site );
		}
		else
		{
			// TODO: Movement
		}
	}

	// Process menu actions
	rftl::vector<input::GameCommand> const menuCommands =
		InputHelpers::GetGameplayInputToProcess( input::HardcodedGetLocalPlayer(), input::layer::GameMenu );
	for( input::GameCommand const& menuCommand : menuCommands )
	{
		if( menuCommand.mType == input::command::game::UIMenuAction )
		{
			// Bring up menus
			context.mManager.RequestDeferredStateChange( id::Gameplay_Menus );
		}
	}

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

	// Process overworld characters
	{
		using namespace state;

		// Get the active party characters
		rftl::array<state::MutableObjectRef, 3> const activePartyCharacters =
			gCompanyManager->FindMutableActivePartyObjects( input::HardcodedGetLocalPlayer() );

		// For each active team member...
		for( size_t i_teamIndex = 0; i_teamIndex < company::kActiveTeamSize; i_teamIndex++ )
		{
			state::MutableObjectRef const& character = activePartyCharacters.at( i_teamIndex );
			if( character.IsSet() == false )
			{
				// Not active
				continue;
			}

			// Find character components
			comp::OverworldVisual& visual = *character.GetMutableComponentInstanceT<comp::OverworldVisual>();
			comp::OverworldMovement& movement = *character.GetMutableComponentInstanceT<comp::OverworldMovement>();

			// Update movement
			{
				// HACK: Face south, split positions out
				// TODO: Input, etc.
				movement.mCurPos.mFacing = comp::OverworldMovement::Pos::Facing::South;
				movement.mCurPos.mMoving = true;
				movement.mCurPos.mX = math::integer_cast<gfx::PPUCoordElem>( i_teamIndex * 32 );
			}

			// Update and draw visuals
			{
				gfx::Object object = {};

				object.mXCoord = movement.mCurPos.mX;
				object.mYCoord = movement.mCurPos.mY;

				// HACK: Sort by reverse team order
				// TODO: Something clever with the Y-axis as input
				object.mZLayer = -math::integer_cast<gfx::PPUDepthLayer>( 5u - i_teamIndex );

				bool const idle = movement.mCurPos.mMoving == false;

				// Animate based on movement
				comp::OverworldVisual::Anim const* anim;
				switch( movement.mCurPos.mFacing )
				{
					case comp::OverworldMovement::Pos::Facing::North:
						anim = idle ? &visual.mIdleNorth : &visual.mWalkNorth;
						break;
					case comp::OverworldMovement::Pos::Facing::East:
						anim = idle ? &visual.mIdleEast : &visual.mWalkEast;
						break;
					case comp::OverworldMovement::Pos::Facing::South:
						anim = idle ? &visual.mIdleSouth : &visual.mWalkSouth;
						break;
					case comp::OverworldMovement::Pos::Facing::West:
						anim = idle ? &visual.mIdleWest : &visual.mWalkWest;
						break;
					default:
						RFLOG_FATAL( character, RFCAT_CC3O3, "Unexpected codepath" );
				}
				object.mFramePackID = anim->mFramePackID;

				// Use timer from component, and update it
				object.mTimer = visual.mTimer;
				object.mTimer.mMaxTimeIndex = anim->mMaxTimeIndex;
				object.mTimer.mTimeSlowdown = anim->mSlowdownRate;
				object.mLooping = true;
				object.Animate();
				visual.mTimer = object.mTimer;

				ppu.DrawObject( object );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

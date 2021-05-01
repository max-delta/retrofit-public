#include "stdafx.h"
#include "Gameplay_Overworld.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/overworld/Overworld.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/components/OverworldVisual.h"
#include "cc3o3/state/components/OverworldMovement.h"
#include "cc3o3/state/StateHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"
#include "GameInput/GameController.h"
#include "GamePixelPhysics/DirectionLogic.h"
#include "GamePixelPhysics/PixelCast.h"
#include "GamePixelPhysics/PrimitiveCollision.h"
#include "GameUI/ContainerManager.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/NineSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/TileLayerCSVLoader.h"
#include "PPU/Viewport.h"

#include "Timing/FrameClock.h"

#include "core_component/TypedObjectRef.h"
#include "core_state/VariableIdentifier.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/static_vector.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Overworld::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	static constexpr gfx::ppu::PPUDepthLayer kLayerTerrain = 100;
	static constexpr gfx::ppu::PPUDepthLayer kLayerRegion = -20;
	static constexpr gfx::ppu::PPUDepthLayer kLayerCharStart = 10;
	static constexpr gfx::ppu::PPUDepthLayer kLayerCloudStart = -50;

	math::Bitmap mCollisionMap{ ExplicitDefaultConstruct{} };
	gfx::ppu::TileLayer mTerrainLand = {};
	gfx::ppu::TileLayer mTerrainCloudA = {};
	gfx::ppu::TileLayer mTerrainCloudB = {};
	overworld::Overworld::Areas mAreas;

	gfx::ppu::Viewport mViewport = {};

	WeakPtr<ui::Controller> mUI;
	time::CommonClock::time_point mLastActivity;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Overworld::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::ppu::PPUController& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	file::VFS& vfs = *app::gVfs;

	// Load map
	// HACK: Hard-coded
	// TODO: Figure out from current pawn position and a world manager
	static constexpr char const kHackMap[] = "island1";
	file::VFSPath const mapDescPath = paths::TablesRoot().GetChild( "world", "overworlds", rftl::string( kHackMap ) + ".oo" );
	overworld::Overworld const map = overworld::Overworld::LoadFromDesc( mapDescPath );

	// Setup collision
	{
		internalState.mCollisionMap = map.mCollisionMap;
	}

	// Setup areas and transitions
	{
		internalState.mAreas = map.mAreas;
	}

	// Setup terrain
	{
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, map.mTerrainTilesetPath );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, map.mCloud1TilesetPath );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, map.mCloud2TilesetPath );

		gfx::ppu::TileLayer& land = internalState.mTerrainLand;
		gfx::ppu::TileLayer& cloudA = internalState.mTerrainCloudA;
		gfx::ppu::TileLayer& cloudB = internalState.mTerrainCloudB;

		land = {};
		land.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( map.mTerrainTilesetPath );
		land.mTileZoomFactor = gfx::ppu::TileLayer::kTileZoomFactor_Normal;
		land.mXCoord = 0;
		land.mYCoord = 0;
		land.mZLayer = InternalState::kLayerTerrain;
		land.mWrapping = false;
		land.mLooping = true;
		land.mTimer.mMaxTimeIndex = 50;
		{
			bool const loadSuccess = gfx::ppu::TileLayerCSVLoader::LoadTiles( land, vfs, map.mTerrainTilemapPath );
			RF_ASSERT( loadSuccess );
		}

		cloudA = {};
		cloudA.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( map.mCloud1TilesetPath );
		cloudA.mTileZoomFactor = gfx::ppu::TileLayer::kTileZoomFactor_Normal;
		cloudA.mXCoord = 0;
		cloudA.mYCoord = 0;
		cloudA.mZLayer = InternalState::kLayerCloudStart - 0;
		cloudA.mWrapping = true;
		cloudA.mLooping = true;
		cloudA.mTimer.mMaxTimeIndex = map.mCloud1ParallaxDelay;
		{
			bool const loadSuccess = gfx::ppu::TileLayerCSVLoader::LoadTiles( cloudA, vfs, map.mCloud1TilemapPath );
			RF_ASSERT( loadSuccess );
		}

		cloudB = {};
		cloudB.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( map.mCloud2TilesetPath );
		cloudB.mTileZoomFactor = gfx::ppu::TileLayer::kTileZoomFactor_Normal;
		cloudB.mXCoord = 0;
		cloudB.mYCoord = 0;
		cloudB.mZLayer = InternalState::kLayerCloudStart - 1;
		cloudB.mWrapping = true;
		cloudB.mLooping = true;
		cloudB.mTimer.mMaxTimeIndex = map.mCloud2ParallaxDelay;
		{
			bool const loadSuccess = gfx::ppu::TileLayerCSVLoader::LoadTiles( cloudB, vfs, map.mCloud2TilemapPath );
			RF_ASSERT( loadSuccess );
		}
	}

	// Setup overworld characters
	{
		using namespace state;

		// Get the active party characters
		rftl::array<state::MutableObjectRef, company::kActiveTeamSize> const activePartyCharacters =
			gCompanyManager->FindMutableActivePartyObjects( InputHelpers::GetSinglePlayer() );

		// For each active team member...
		for( size_t i_teamIndex = 0; i_teamIndex < company::kActiveTeamSize; i_teamIndex++ )
		{
			state::MutableObjectRef const& character = activePartyCharacters.at( i_teamIndex );
			if( character.IsSet() == false )
			{
				// Not active
				continue;
			}

			// Clear movement
			comp::OverworldMovement& movement = *character.GetMutableComponentInstanceT<comp::OverworldMovement>();
			movement.mCurPos.mPrimary = phys::Direction::South;
			movement.mCurPos.mSecondary = phys::Direction::Invalid;
			movement.mCurPos.mLatent = phys::Direction::Invalid;
			movement.mCurPos.mMoving = false;
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
			true, false, false };
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
					math::integer_cast<gfx::ppu::PPUCoordElem>( gfx::ppu::kTileSize * 3 ),
					math::integer_cast<gfx::ppu::PPUCoordElem>( gfx::ppu::kTileSize ),
					ui::Justification::TopRight ) );
		headerFloater->SetOffset( uiContext, { -gfx::ppu::kTileSize / 4, gfx::ppu::kTileSize / 4 } );
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
					math::integer_cast<gfx::ppu::PPUCoordElem>( gfx::ppu::kTileSize * 2 ),
					math::integer_cast<gfx::ppu::PPUCoordElem>( gfx::ppu::kTileSize / 2 ),
					ui::Justification::BottomLeft ) );
		footerFloater->SetOffset( uiContext, { gfx::ppu::kTileSize / 4, -gfx::ppu::kTileSize / 4 } );
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
	gfx::ppu::PPUController& ppu = *app::gGraphics;

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

	// Prepare for movement
	using MovementPtr = WeakPtr<state::comp::OverworldMovement>;
	using MovementPtrs = rftl::static_vector<MovementPtr, company::kActiveTeamSize>;
	MovementPtr pawnMovementPtr;
	MovementPtrs followerMovementPtrs;
	{
		// Get the active party characters
		rftl::array<state::MutableObjectRef, company::kActiveTeamSize> const activePartyCharacters =
			gCompanyManager->FindMutableActivePartyObjects( InputHelpers::GetSinglePlayer() );

		// For each active team member...
		for( size_t i_teamIndex = 0; i_teamIndex < company::kActiveTeamSize; i_teamIndex++ )
		{
			state::MutableObjectRef const& character = activePartyCharacters.at( i_teamIndex );
			if( character.IsSet() == false )
			{
				// Not active
				continue;
			}

			// Get movement
			MovementPtr const movementPtr = character.GetMutableComponentInstanceT<state::comp::OverworldMovement>();
			RF_ASSERT( movementPtr != nullptr );
			if( pawnMovementPtr == nullptr )
			{
				pawnMovementPtr = movementPtr;
			}
			else
			{
				followerMovementPtrs.emplace_back( movementPtr );
			}
		}
	}
	RF_ASSERT( pawnMovementPtr != nullptr );

	// Want to determine movement intent
	auto const onMoveIntent = //
		[&pawnMovementPtr] //
		( phys::Direction::Value moveStart, phys::Direction::Value moveStop ) -> void //
	{
		using namespace enable_bitwise_enums;

		RF_ASSERT( pawnMovementPtr != nullptr );
		state::comp::OverworldMovement& pawnMovement = *pawnMovementPtr;

		// Will update movement direction
		state::comp::OverworldMovement::Pos& curPos = pawnMovement.mCurPos;

		// Calc movement layers
		phys::Direction::Value primary;
		phys::Direction::Value secondary;
		phys::Direction::Value latent;
		if( curPos.mMoving )
		{
			primary = curPos.mPrimary;
			secondary = curPos.mSecondary;
			latent = curPos.mLatent;
			RF_ASSERT( primary != phys::Direction::Invalid );
		}
		else
		{
			primary = phys::Direction::Invalid;
			secondary = phys::Direction::Invalid;
			latent = phys::Direction::Invalid;
		}
		phys::DirectionLogic::UpdateStack(
			primary, secondary, latent,
			moveStart, moveStop );

		// Are we moving?
		phys::Direction::Value desiredMovement = primary | secondary;
		if( desiredMovement == phys::Direction::Invalid )
		{
			// Standing only, not actually moving
			RF_ASSERT( primary == phys::Direction::Invalid );
			( (void)curPos.mPrimary ); // Don't change, used for visuals
			RF_ASSERT( secondary == phys::Direction::Invalid );
			curPos.mSecondary = secondary;
			RF_ASSERT( latent == phys::Direction::Invalid );
			curPos.mLatent = latent;
			curPos.mMoving = false;
		}
		else
		{
			// Moving
			RF_ASSERT( primary != phys::Direction::Invalid );
			curPos.mPrimary = primary;
			curPos.mSecondary = secondary;
			curPos.mLatent = latent;
			curPos.mMoving = true;
		}
	};
	auto const onMoveStart = [&onMoveIntent]( phys::Direction::Value direction ) -> void //
	{
		onMoveIntent( direction, phys::Direction::Invalid );
	};
	auto const onMoveStop = [&onMoveIntent]( phys::Direction::Value direction ) -> void //
	{
		onMoveIntent( phys::Direction::Invalid, direction );
	};

	// Process character control
	rftl::vector<input::GameCommand> const charCommands =
		InputHelpers::GetGameplayInputToProcess( InputHelpers::GetSinglePlayer(), input::layer::CharacterControl );
	for( input::GameCommand const& charCommand : charCommands )
	{
		using namespace enable_bitwise_enums;

		// Treat as activity
		internalState.mLastActivity = time::FrameClock::now();

		if( charCommand.mType == input::command::game::Interact )
		{
			// HACK: Just pop into site
			// TODO: Make sure party is actually at a site, and set it up
			context.mManager.RequestDeferredStateChange( id::Gameplay_Site );
		}
		else if( charCommand.mType == input::command::game::WalkNorth )
		{
			onMoveStart( phys::Direction::North );
		}
		else if( charCommand.mType == input::command::game::WalkNorthStop )
		{
			onMoveStop( phys::Direction::North );
		}
		else if( charCommand.mType == input::command::game::WalkEast )
		{
			onMoveStart( phys::Direction::East );
		}
		else if( charCommand.mType == input::command::game::WalkEastStop )
		{
			onMoveStop( phys::Direction::East );
		}
		else if( charCommand.mType == input::command::game::WalkSouth )
		{
			onMoveStart( phys::Direction::South );
		}
		else if( charCommand.mType == input::command::game::WalkSouthStop )
		{
			onMoveStop( phys::Direction::South );
		}
		else if( charCommand.mType == input::command::game::WalkWest )
		{
			onMoveStart( phys::Direction::West );
		}
		else if( charCommand.mType == input::command::game::WalkWestStop )
		{
			onMoveStop( phys::Direction::West );
		}
	}

	// Process menu actions
	rftl::vector<input::GameCommand> const menuCommands =
		InputHelpers::GetGameplayInputToProcess( InputHelpers::GetSinglePlayer(), input::layer::GameMenu );
	for( input::GameCommand const& menuCommand : menuCommands )
	{
		if( menuCommand.mType == input::command::game::UIMenuAction )
		{
			// Bring up menus
			context.mManager.RequestDeferredStateChange( id::Gameplay_Menus );
		}
	}

	// Update movement
	{
		RF_ASSERT( pawnMovementPtr != nullptr );
		state::comp::OverworldMovement& pawnMovement = *pawnMovementPtr;

		// Update main position
		if( pawnMovement.mCurPos.mMoving )
		{
			using namespace enable_bitwise_enums;

			phys::Coord pos = pawnMovement.mCurPos.GetCoord();
			phys::Direction::Value dir = pawnMovement.mCurPos.mPrimary.As() | pawnMovement.mCurPos.mSecondary.As();

			// Step
			static constexpr bool kDoubleSpeed = false;
			pos = phys::PixelCast::FixOutOfBounds( internalState.mCollisionMap, pos );
			pos = phys::PixelCast::SlideStepCast( internalState.mCollisionMap, pos, dir );
			if constexpr( kDoubleSpeed )
			{
				// Make second step more restrictive, giving the effect of
				//  sliding against walls cause the feeling of half-speed
				pos = phys::PixelCast::CardinalStepCast( internalState.mCollisionMap, pos, dir );
			}

			pawnMovement.mCurPos.SetCoord( pos );
		}

		RF_TODO_ANNOTATION( "Move followers" );

		// Update viewport
		static constexpr gfx::ppu::PPUCoordElem kViewportMarginX = gfx::ppu::kTileSize * 4;
		static constexpr gfx::ppu::PPUCoordElem kViewportMarginY = gfx::ppu::kTileSize * 3;
		gfx::ppu::AABB const mapLimits = {
			gfx::ppu::PPUCoord{ 0, 0 },
			gfx::ppu::PPUCoord{
				math::integer_cast<gfx::ppu::PPUCoordElem>( internalState.mCollisionMap.GetWidth() ),
				math::integer_cast<gfx::ppu::PPUCoordElem>( internalState.mCollisionMap.GetHeight() ) } };
		ppu.UpdateViewportExtents( internalState.mViewport );
		internalState.mViewport.SlideToFit( pawnMovement.mCurPos.GetCoord(), kViewportMarginX, kViewportMarginY );
		internalState.mViewport.ClampToWithin( mapLimits );
	}

	// Apply viewport
	ppu.ApplyViewport( internalState.mViewport );

	// Draw terrain
	{
		gfx::ppu::TileLayer& land = internalState.mTerrainLand;
		gfx::ppu::TileLayer& cloudA = internalState.mTerrainCloudA;
		gfx::ppu::TileLayer& cloudB = internalState.mTerrainCloudB;

		constexpr auto parralax =
			[](
				gfx::ppu::PPUController const& ppu,
				gfx::ppu::TileLayer& tileLayer ) -> void //
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

	// Draw areas
	{
		RF_ASSERT( pawnMovementPtr != nullptr );
		phys::Coord const curPos = pawnMovementPtr->mCurPos.GetCoord();

		for( overworld::Area const& area : internalState.mAreas )
		{
			bool const inArea = phys::PrimitiveCollision::HasCollision( area.mAABB, curPos );

			// TODO: Configurable debug rendering
			ppu.DebugDrawAABB( area.mAABB, inArea ? 2 : 1, InternalState::kLayerRegion, math::Color3f::kCyan );
			ppu.DebugDrawText( area.mFocus, "%s", area.mIdentifier.c_str() );
		}
	}

	// Draw overworld characters
	{
		using namespace state;

		// Get the active party characters
		rftl::array<state::MutableObjectRef, company::kActiveTeamSize> const activePartyCharacters =
			gCompanyManager->FindMutableActivePartyObjects( InputHelpers::GetSinglePlayer() );

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
			comp::OverworldMovement const& movement = *character.GetComponentInstanceT<comp::OverworldMovement>();

			// Update and draw visuals
			{
				gfx::ppu::Object object = {};

				object.mXCoord = movement.mCurPos.mX;
				object.mYCoord = movement.mCurPos.mY;

				// HACK: Sort by reverse team order
				// TODO: Something clever with the Y-axis as input
				object.mZLayer = InternalState::kLayerCharStart - math::integer_cast<gfx::ppu::PPUDepthLayer>( i_teamIndex );

				bool const idle = movement.mCurPos.mMoving == false;

				// Animate based on movement
				comp::OverworldVisual::Anim const* anim;
				switch( movement.mCurPos.mPrimary )
				{
					case phys::Direction::North:
						anim = idle ? &visual.mIdleNorth : &visual.mWalkNorth;
						break;
					case phys::Direction::East:
						anim = idle ? &visual.mIdleEast : &visual.mWalkEast;
						break;
					case phys::Direction::South:
						anim = idle ? &visual.mIdleSouth : &visual.mWalkSouth;
						break;
					case phys::Direction::West:
						anim = idle ? &visual.mIdleWest : &visual.mWalkWest;
						break;
					case phys::Direction::NE:
					case phys::Direction::NW:
					case phys::Direction::SE:
					case phys::Direction::SW:
					case phys::Direction::Invalid:
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
}

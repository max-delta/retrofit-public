#include "stdafx.h"
#include "DevTestRollback.h"

#include "cc3o3/input/InputFwd.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/time/TimeFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"
#include "GameUI/FontRegistry.h"
#include "GameAppState/AppStateTickContext.h"
#include "GameSync/RollbackFilters.h"
#include "GameSync/RollbackController.h"

#include "PPU/PPUController.h"

#include "Rollback/RollbackManager.h"
#include "Rollback/AutoVar.h"

#include "Timing/FrameClock.h"

#include "core_allocate/LinearAllocator.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestRollback::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	void Bind( rollback::Window& window, state::VariableIdentifier const& parent )
	{
		mP1.Bind( window, parent.GetChild( "p1" ), mAlloc );
		mP2.Bind( window, parent.GetChild( "p2" ), mAlloc );
		mP3.Bind( window, parent.GetChild( "p3" ), mAlloc );
		mP4.Bind( window, parent.GetChild( "p4" ), mAlloc );
	}

	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<2048>> mAlloc{ ExplicitDefaultConstruct() };

	struct Pos
	{
		RF_NO_COPY( Pos );
		Pos() = default;

		void Bind( rollback::Window& window, state::VariableIdentifier const& parent, alloc::Allocator& allocator )
		{
			mX.Bind( window, parent.GetChild( "x" ), allocator );
			mY.Bind( window, parent.GetChild( "y" ), allocator );
		}

		rollback::AutoVar<uint8_t> mX;
		rollback::AutoVar<uint8_t> mY;
	};
	Pos mP1;
	Pos mP2;
	Pos mP3;
	Pos mP4;

	UniquePtr<input::RollbackController> mP3RollbackController;
	UniquePtr<input::RollbackController> mP4RollbackController;
};

///////////////////////////////////////////////////////////////////////////////

void DevTestRollback::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;
	rollback::RollbackManager& rollMan = *app::gRollbackManager;
	gfx::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( { 0.f, 0.f, 1.f } );

	rollback::Window& window = rollMan.GetMutableSharedDomain().GetMutableWindow();
	internalState.Bind( window, state::VariableIdentifier( "DevTest", "Rollback" ) );

	rollMan.CreateNewStream( 3, rollMan.GetHeadClock() );
	internalState.mP3RollbackController = DefaultCreator<input::RollbackController>::Create();
	internalState.mP3RollbackController->SetRollbackManager( app::gRollbackManager );
	internalState.mP3RollbackController->SetRollbackIdentifier( 3 );

	rollMan.CreateNewStream( 4, rollMan.GetHeadClock() );
	internalState.mP4RollbackController = DefaultCreator<input::RollbackController>::Create();
	internalState.mP4RollbackController->SetRollbackManager( app::gRollbackManager );
	internalState.mP4RollbackController->SetRollbackIdentifier( 4 );
}



void DevTestRollback::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestRollback::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;

	gfx::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, char const* fmt, ... ) -> bool {
		gfx::PPUCoord const pos = gfx::PPUCoord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, -1, font.mFontHeight, font.mManagedFontID, false, math::Color3f::kWhite, fmt, args );
		va_end( args );
		return retVal;
	};


	drawText( 1, 1, "DEV TEST - ROLLBACK" );
	drawText( 2, 3, "P1x: %u", internalState.mP1.mX.As() );
	drawText( 2, 4, "P1y: %u", internalState.mP1.mY.As() );
	drawText( 2, 5, "P2x: %u", internalState.mP2.mX.As() );
	drawText( 2, 6, "P2y: %u", internalState.mP2.mY.As() );
	drawText( 2, 7, "P3x: %u", internalState.mP3.mX.As() );
	drawText( 2, 8, "P3y: %u", internalState.mP3.mY.As() );
	drawText( 2, 9, "P4x: %u", internalState.mP4.mX.As() );
	drawText( 2, 10, "P4y: %u", internalState.mP4.mY.As() );

	input::ControllerManager const& controllerManager = *app::gInputControllerManager;

	static constexpr input::PlayerID kPlayerIDs[] = {
		input::player::P1,
		input::player::P2,
		3,
		4
	};
	InternalState::Pos* const positions[] = {
		&internalState.mP1,
		&internalState.mP2,
		&internalState.mP3,
		&internalState.mP4
	};

	input::GameController const* const controllers[] = {
		controllerManager.GetGameController( input::player::P1, input::layer::CharacterControl ),
		controllerManager.GetGameController( input::player::P2, input::layer::CharacterControl ),
		internalState.mP3RollbackController,
		internalState.mP4RollbackController,
	};

	rollback::RollbackManager& rollMan = *app::gRollbackManager;

	// P3 will be a clone of P2
	rollback::InputStreamRef const p3Stream = sync::RollbackFilters::GetMutableStreamRef( rollMan, 3 );
	sync::RollbackFilters::PrepareLocalFrame( rollMan, p3Stream, context.mCurrentTime );

	// P3 will be a clone of P2, but as rollback triggers
	rollback::InputStreamRef const p4Stream = sync::RollbackFilters::GetMutableStreamRef( rollMan, 4 );
	static constexpr time::CommonClock::duration kLateDuration = time::kSimulationFrameDuration * 7;
	bool const p4Valid = sync::RollbackFilters::TryPrepareRemoteFrame( rollMan, p4Stream, context.mCurrentTime - kLateDuration );

	for( size_t i = 0; i < 4; i++ )
	{
		input::PlayerID const playerID = kPlayerIDs[i];
		InternalState::Pos* const pos = positions[i];
		input::GameController const& controller = *controllers[i];

		// Fetch commands that were entered for this current frame
		rftl::vector<input::GameCommand> commands;
		rftl::virtual_back_inserter_iterator<input::GameCommand, decltype( commands )> parser( commands );
		controller.GetGameCommandStream( parser, time::FrameClock::now(), time::FrameClock::now() );

		for( input::GameCommand const& command : commands )
		{
			if( playerID == input::player::P2 )
			{
				// Clone player 2's commands onto player 3
				p3Stream.second.emplace_back( rollback::InputEvent( command.mTime, command.mType ) );

				// Clone player 2's commands onto player 4
				if( p4Valid )
				{
					p4Stream.second.emplace_back( rollback::InputEvent( command.mTime - kLateDuration, command.mType ) );
				}
			}

			if( command.mType == input::command::game::WalkWest )
			{
				pos->mX = pos->mX - 1u;
			}
			else if( command.mType == input::command::game::WalkEast )
			{
				pos->mX = pos->mX + 1u;
			}
			else if( command.mType == input::command::game::WalkNorth )
			{
				pos->mY = pos->mY - 1u;
			}
			else if( command.mType == input::command::game::WalkSouth )
			{
				pos->mY = pos->mY + 1u;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

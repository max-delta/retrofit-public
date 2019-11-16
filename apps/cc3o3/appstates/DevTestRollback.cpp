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

	void Bind( rollback::Window& window )
	{
		mP1.mX.Bind( window, "DevTest/Rollback/p1/x", mAlloc );
		mP1.mY.Bind( window, "DevTest/Rollback/p1/y", mAlloc );
		mP2.mX.Bind( window, "DevTest/Rollback/p2/x", mAlloc );
		mP2.mY.Bind( window, "DevTest/Rollback/p2/y", mAlloc );
	}

	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<1024>> mAlloc{ ExplicitDefaultConstruct() };

	struct Pos
	{
		RF_NO_COPY( Pos );
		Pos() = default;
		rollback::AutoVar<uint8_t> mX;
		rollback::AutoVar<uint8_t> mY;
	};
	Pos mP1;
	Pos mP2;
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
	internalState.Bind( window );

	rollMan.CreateNewStream( 3, rollMan.GetHeadClock() );
	rollMan.CreateNewStream( 4, rollMan.GetHeadClock() );
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

	input::ControllerManager const& controllerManager = *app::gInputControllerManager;

	static constexpr input::PlayerID kPlayerIDs[] = { input::player::P1, input::player::P2 };
	InternalState::Pos* const positions[] = { &internalState.mP1, &internalState.mP2 };

	rollback::RollbackManager& rollMan = *app::gRollbackManager;

	// P3 will be a clone of P2
	rollback::InputStreamRef const p3Stream = sync::RollbackFilters::GetMutableStreamRef( rollMan, 3 );
	sync::RollbackFilters::PrepareLocalFrame( rollMan, p3Stream, context.mCurrentTime );

	// P3 will be a clone of P2, but as rollback triggers
	rollback::InputStreamRef const p4Stream = sync::RollbackFilters::GetMutableStreamRef( rollMan, 4 );
	static constexpr time::CommonClock::duration kLateDuration = time::kSimulationFrameDuration * 7;
	bool const p4Valid = sync::RollbackFilters::TryPrepareRemoteFrame( rollMan, p4Stream, context.mCurrentTime - kLateDuration );

	for( size_t i = 0; i < 2; i++ )
	{
		input::PlayerID const playerID = kPlayerIDs[i];
		InternalState::Pos* const pos = positions[i];

		input::GameController const& controller = *controllerManager.GetGameController( playerID, input::layer::CharacterControl );

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

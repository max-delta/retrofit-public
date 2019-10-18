#include "stdafx.h"
#include "DevTestRollback.h"

#include "cc3o3/input/InputFwd.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "Rollback/RollbackManager.h"
#include "Rollback/Var.h"

#include "Timing/FrameClock.h"

#include "core_allocate/LinearAllocator.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestRollback::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<512>> mAlloc{ ExplicitDefaultConstruct() };

	static constexpr char kP1x[] = "DevTest/Rollback/p1/x";
	static constexpr char kP1y[] = "DevTest/Rollback/p1/y";

	rollback::Var<uint8_t> mP1x;
	rollback::Var<uint8_t> mP1y;
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
	internalState.mP1x = window.GetOrCreateStream<uint8_t>( InternalState::kP1x, internalState.mAlloc );
	internalState.mP1y = window.GetOrCreateStream<uint8_t>( InternalState::kP1y, internalState.mAlloc );
}



void DevTestRollback::OnExit( AppStateChangeContext& context )
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;
	rollback::Window& window = rollMan.GetMutableSharedDomain().GetMutableWindow();
	window.RemoveStream<uint8_t>( InternalState::kP1x );
	window.RemoveStream<uint8_t>( InternalState::kP1y );

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
	drawText( 2, 3, "P1x: %u", internalState.mP1x.As() );
	drawText( 2, 4, "P1y: %u", internalState.mP1y.As() );

	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	input::GameController const& controller = *controllerManager.GetGameController( input::player::P1, input::layer::CharacterControl );

	// Fetch commands that were entered for this current frame
	rftl::vector<input::GameCommand> commands;
	rftl::virtual_back_inserter_iterator<input::GameCommand, decltype( commands )> parser( commands );
	controller.GetGameCommandStream( parser, time::FrameClock::now(), time::FrameClock::now() );

	for( input::GameCommand const& command : commands )
	{
		if( command.mType == input::command::game::WalkWest )
		{
			internalState.mP1x = internalState.mP1x - 1u;
		}
		else if( command.mType == input::command::game::WalkEast )
		{
			internalState.mP1x = internalState.mP1x + 1u;
		}
		else if( command.mType == input::command::game::WalkNorth )
		{
			internalState.mP1y = internalState.mP1y - 1u;
		}
		else if( command.mType == input::command::game::WalkSouth )
		{
			internalState.mP1y = internalState.mP1y + 1u;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

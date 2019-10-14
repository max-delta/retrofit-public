#include "stdafx.h"
#include "DevTestRollback.h"

#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "Rollback/RollbackManager.h"
#include "Timing/FrameClock.h"

#include "core_allocate/LinearAllocator.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestRollback::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;
	~InternalState()
	{
		// Need to release weak pointers before destroying allocator
		mVars = {};
	}

	struct Vars
	{
		static constexpr char kP1x[] = "DevTest/Rollback/p1/x";
		static constexpr char kP1y[] = "DevTest/Rollback/p1/y";

		WeakPtr<rollback::Var<uint8_t>> mP1x;
		WeakPtr<rollback::Var<uint8_t>> mP1y;
	} mVars;

	alloc::AllocatorT<alloc::LinearAllocator<4096>> mAlloc{ ExplicitDefaultConstruct() };
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
	internalState.mVars.mP1x = window.GetOrCreateStream<uint8_t>( InternalState::Vars::kP1x, internalState.mAlloc );
	internalState.mVars.mP1y = window.GetOrCreateStream<uint8_t>( InternalState::Vars::kP1y, internalState.mAlloc );
}



void DevTestRollback::OnExit( AppStateChangeContext& context )
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;
	rollback::Window& window = rollMan.GetMutableSharedDomain().GetMutableWindow();
	window.RemoveStream<uint8_t>( InternalState::Vars::kP1x );
	window.RemoveStream<uint8_t>( InternalState::Vars::kP1y );

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
	drawText( 2, 3, "P1x: %u", internalState.mVars.mP1x->Read( time::FrameClock::now() ) );
	drawText( 2, 4, "P1y: %u", internalState.mVars.mP1y->Read( time::FrameClock::now() ) );

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		if( focusEvent == ui::focusevent::Command_NavigateLeft )
		{
			internalState.mVars.mP1x->Write( time::FrameClock::now(), internalState.mVars.mP1x->Read( time::FrameClock::now() ) - 1u );
		}
		else if( focusEvent == ui::focusevent::Command_NavigateRight )
		{
			internalState.mVars.mP1x->Write( time::FrameClock::now(), internalState.mVars.mP1x->Read( time::FrameClock::now() ) + 1u );
		}
		else if( focusEvent == ui::focusevent::Command_NavigateUp )
		{
			internalState.mVars.mP1y->Write( time::FrameClock::now(), internalState.mVars.mP1y->Read( time::FrameClock::now() ) - 1u );
		}
		else if( focusEvent == ui::focusevent::Command_NavigateDown )
		{
			internalState.mVars.mP1y->Write( time::FrameClock::now(), internalState.mVars.mP1y->Read( time::FrameClock::now() ) + 1u );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

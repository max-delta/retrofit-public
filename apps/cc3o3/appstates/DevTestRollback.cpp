#include "stdafx.h"
#include "DevTestRollback.h"

#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestRollback::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	size_t mP1x = 0;
	size_t mP1y = 0;
};

///////////////////////////////////////////////////////////////////////////////

void DevTestRollback::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	//InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( { 0.f, 0.f, 1.f } );
}



void DevTestRollback::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestRollback::OnTick( AppStateTickContext& context )
{
	//InternalState& internalState = *mInternalState;

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

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		// No handlers currently
		(void)focusEvent;
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

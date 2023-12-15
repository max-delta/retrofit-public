#include "stdafx.h"
#include "DevTestElementLab.h"

#include "cc3o3/Common.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/casting/CastError.h"
#include "cc3o3/casting/CastingEngine.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestElementLab::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	// TODO
};

///////////////////////////////////////////////////////////////////////////////

void DevTestElementLab::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	//InternalState& internalState = *mInternalState;
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( { 0.f, 0.f, 1.f } );
}



void DevTestElementLab::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestElementLab::OnTick( AppStateTickContext& context )
{
	using namespace combat;

	InternalState& internalState = *mInternalState;

	gfx::ppu::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, char const* fmt, ... ) -> bool //
	{
		gfx::ppu::Coord const pos = gfx::ppu::Coord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, -1, font.mFontHeight, font.mManagedFontID, false, math::Color3f::kWhite, fmt, args );
		va_end( args );
		return retVal;
	};


	drawText( 1, 1, "DEV TEST - ELEMENT LAB" );

	// Setup initial combat instance
	combat::CombatInstance initialCombatInstance( gCombatEngine );
	combat::FighterID source = {};
	combat::FighterID target = {};
	{
		// TODO: Setup initial combat instance
	}

	// TODO: Have a way to choose or configure this
	element::ElementIdentifier elementToCast =
		element::MakeElementIdentifier( "ST_R_1" );

	// Cast the element to create a new combat instance
	combat::CombatInstance resultingCombatInstance = initialCombatInstance;
	UniquePtr<cast::CastError> const castError = gCastingEngine->ExecuteElementCast(
		resultingCombatInstance,
		source,
		target,
		elementToCast );

	// TODO: Will want to display error information
	RF_ASSERT( castError == cast::CastError::kNoError );

	// TODO
	( (void)internalState );

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		// TODO
		( (void)focusEvent );
	}

	uint8_t x = 2;
	uint8_t y = 2;
	drawText( x, y, "TODO" );
	y++;
	drawText( x, y, "TODO" );
	y++;
}

///////////////////////////////////////////////////////////////////////////////
}

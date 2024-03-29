#include "stdafx.h"
#include "DevTestGridCharts.h"

#include "cc3o3/Common.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/char/GridSizeMask.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/integer_literals.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestGridCharts::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	size_t mCursor = 0;

	uint8_t mStoryTier = 3;
	character::StatValue mElemPower = 2;
	character::GridShape mGridShape = character::GridShape::Standard;
};

///////////////////////////////////////////////////////////////////////////////

void DevTestGridCharts::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	//InternalState& internalState = *mInternalState;
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( { 0.f, 0.f, 1.f } );
}



void DevTestGridCharts::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestGridCharts::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	using StatValue = character::StatValue;
	using GridShape = character::GridShape;

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


	drawText( 1, 1, "DEV TEST - GRID CHARTS" );

	size_t& cursor = internalState.mCursor;
	uint8_t& storyTier = internalState.mStoryTier;
	StatValue& elemPower = internalState.mElemPower;
	GridShape& gridShape = internalState.mGridShape;

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		static constexpr size_t kMaxCursorPos = 2;

		if( focusEvent == ui::focusevent::Command_NavigateLeft )
		{
			cursor--;
		}
		else if( focusEvent == ui::focusevent::Command_NavigateRight )
		{
			cursor++;
		}
		cursor = math::Clamp<size_t>( 0, cursor, kMaxCursorPos );

		bool increase = focusEvent == ui::focusevent::Command_NavigateUp;
		bool decrease = focusEvent == ui::focusevent::Command_NavigateDown;
		if( increase || decrease )
		{
			RF_ASSERT( increase != decrease );
			switch( cursor )
			{
				case 0:
					storyTier += increase ? 1 : -1;
					break;
				case 1:
					elemPower += increase ? 1 : -1;
					break;
				case 2:
				{
					uint8_t temp = static_cast<uint8_t>( static_cast<int8_t>( gridShape ) + ( increase ? 1 : -1 ) );
					if( temp > character::kMaxGridShapeValue )
					{
						temp = 0;
					}
					gridShape = static_cast<GridShape>( temp );
					break;
				}
				default:
					break;
			}
		}
	}

	uint8_t x = 2;
	uint8_t y = 2;
	switch( cursor )
	{
		case 0:
			drawText( x, y, "*story  power  shape" );
			break;
		case 1:
			drawText( x, y, " story *power  shape" );
			break;
		case 2:
			drawText( x, y, " story  power *shape" );
			break;
		default:
			break;
	}
	y++;
	char const* shapeStr = "INVALID";
	switch( gridShape )
	{
		case GridShape::Standard:
			shapeStr = "STND";
			break;
		case GridShape::Wide:
			shapeStr = "WIDE";
			break;
		case GridShape::Heavy:
			shapeStr = "HEVY";
			break;
		case GridShape::NumShapeTypes:
		default:
			RF_DBGFAIL();
			break;
	}
	drawText( x, y, " %5i  %5i  %s", storyTier, elemPower, shapeStr );

	// Grid
	uint8_t const xStart = 5;
	uint8_t const yStart = 7;
	uint8_t const xstep = 2;
	uint8_t const ystep = 1;
	x = xStart;
	y = yStart;
	{
		using character::GridSizeMask;
		GridSizeMask const minSlots = GridSizeMask::CalcMinimumSlots( storyTier );
		GridSizeMask const curSlots = GridSizeMask::CalcSlots( elemPower, gridShape, storyTier );

		for( element::ElementLevel const& level : element::kElementLevels )
		{
			drawText( x, y, " %i", level );
			y += ystep;
			y += ystep;

			size_t const& minCount = minSlots.GetNumSlotsAtLevel( level );
			size_t const& curCount = curSlots.GetNumSlotsAtLevel( level );

			for( size_t slot = 0; slot < character::kMaxSlotsPerElementLevel; slot++ )
			{
				if( slot < minCount )
				{
					drawText( x, y, " @" );
				}
				else if( slot < curCount )
				{
					drawText( x, y, " O" );
				}
				y += ystep;
			}
			x += xstep;
			y = yStart;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}

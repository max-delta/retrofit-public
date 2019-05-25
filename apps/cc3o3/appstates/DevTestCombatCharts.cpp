#include "stdafx.h"
#include "DevTestCombatCharts.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/combat/CombatEngine.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/static_array.h"
#include "rftl/extension/integer_literals.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestCombatCharts::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	combat::CombatEngine mCombatEngine = { app::gVfs };
};

///////////////////////////////////////////////////////////////////////////////

void DevTestCombatCharts::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	//InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( { 0.f, 0.f, 1.f } );
}



void DevTestCombatCharts::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestCombatCharts::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	combat::CombatEngine const& combatEngine = internalState.mCombatEngine;
	using SimVal = combat::CombatEngine::SimVal;
	using SimDelta = combat::CombatEngine::SimDelta;
	using SimColor = combat::CombatEngine::SimColor;

	gfx::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, char const* fmt, ... ) -> bool
	{
		gfx::PPUCoord const pos = { x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) };
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, -1, font.mFontHeight, font.mManagedFontID, false, math::Color3f::kWhite, fmt, args );
		va_end( args );
		return retVal;
	};


	drawText( 1, 1, "DEV TEST - COMBAT CHARTS" );

	// Baseline accuracy
	uint8_t const xStart = 1;
	uint8_t const yStart = 5;
	uint8_t const xstep = 7;
	uint8_t const ystep = 1;
	uint8_t x = xStart;
	uint8_t y = yStart;
	{
		using Swings = rftl::static_array<SimVal, 7>;
		using Hits = rftl::static_array<bool, 7>;
		using Damages = rftl::static_array<SimVal, 7>;
		using namespace rftl::literals;

		rftl::static_array<Swings, 11> combos;
		combos.emplace_back( Swings{ 1_u8 } );
		combos.emplace_back( Swings{ 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 1_u8 } );
		combos.emplace_back( Swings{ 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 2_u8 } );
		combos.emplace_back( Swings{ 1_u8, 1_u8, 1_u8, 2_u8, 2_u8 } );
		combos.emplace_back( Swings{ 1_u8, 1_u8, 2_u8, 3_u8 } );
		combos.emplace_back( Swings{ 1_u8, 2_u8, 2_u8, 2_u8 } );
		combos.emplace_back( Swings{ 1_u8, 2_u8, 3_u8, 1_u8 } );
		combos.emplace_back( Swings{ 1_u8, 3_u8, 3_u8 } );
		combos.emplace_back( Swings{ 2_u8, 2_u8, 3_u8 } );
		combos.emplace_back( Swings{ 2_u8, 3_u8, 2_u8 } );
		combos.emplace_back( Swings{ 3_u8, 3_u8, 1_u8 } );

		for( Swings const& combo : combos )
		{
			Swings const& swings = combo;
			Hits hits;
			Damages damages;

			SimVal const tech = 2;
			SimVal const atk = 2;
			SimVal const atkField = 0;
			SimVal comboMeter = 0;

			SimVal const balance = 2;
			SimVal const def = 2;
			SimVal const defField = 0;
			SimVal counterGuage = 0;

			SimColor const color = SimColor::Unrelated;

			size_t const numSwings = swings.size();
			hits.resize( numSwings );
			damages.resize( numSwings );
			SimVal damageTotal = 0;
			for( size_t i = 0; i < numSwings; i++ )
			{
				SimVal const& swing = swings.at( i );
				bool& hit = hits.at( i );
				SimVal& damage = damages.at( i );

				SimVal const acc = combatEngine.LoCalcAttackAccuracy( swing );
				if( i == 0 )
				{
					comboMeter = combatEngine.LoCalcNewComboMeter( acc, tech );
				}
				else
				{
					comboMeter = combatEngine.LoCalcContinueComboMeter( comboMeter, acc, tech );
				}
				hit = combatEngine.LoCalcWillAttackHit( comboMeter, balance );
				damage = combatEngine.LoCalcAttackDamage( atk, def, swing, color, atkField, defField );

				if( hit )
				{
					damageTotal += damage;
					counterGuage += combatEngine.LoCalcCounterFromAttackSwing( comboMeter );
					counterGuage += combatEngine.LoCalcCounterFromAttackDamage( damage );
					drawText( x, y, " %i:%2i", swing, damage );
					y += ystep;
				}
				else
				{
					damageTotal += 0;
					counterGuage += combatEngine.LoCalcCounterFromAttackSwing( comboMeter );
					counterGuage += 0;
					drawText( x, y, " %i:--", swing );
					y += ystep;
				}
			}
			drawText( x, y, "======" );
			y += ystep;
			drawText( x, y, "ME:%3i", comboMeter );
			y += ystep;
			drawText( x, y, "======" );
			y += ystep;
			drawText( x, y, "CG:%3i", counterGuage );
			y += ystep;
			drawText( x, y, "TO:%3i", damageTotal );
			y += ystep;
			drawText( x, y, "DS:%3i", damageTotal / numSwings );
			y += ystep;

			x += xstep;
			y = yStart;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

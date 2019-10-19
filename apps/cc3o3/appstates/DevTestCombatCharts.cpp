#include "stdafx.h"
#include "DevTestCombatCharts.h"

#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/combat/CombatEngine.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/integer_literals.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestCombatCharts::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	combat::CombatEngine mCombatEngine = { app::gVfs };

	size_t mCursor = 0;

	combat::CombatEngine::SimVal mTech = 2;
	combat::CombatEngine::SimVal mAtk = 2;
	combat::CombatEngine::SimDelta mAtkField = 0;
	combat::CombatEngine::SimVal mBalance = 2;
	combat::CombatEngine::SimVal mDef = 2;
	combat::CombatEngine::SimDelta mDefField = 0;
	combat::CombatEngine::SimColor mColor = combat::CombatEngine::SimColor::Unrelated;
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
	using FieldColors = combat::CombatEngine::FieldColors;

	gfx::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, char const* fmt, ... ) -> bool
	{
		gfx::PPUCoord const pos = gfx::PPUCoord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, -1, font.mFontHeight, font.mManagedFontID, false, math::Color3f::kWhite, fmt, args );
		va_end( args );
		return retVal;
	};


	drawText( 1, 1, "DEV TEST - COMBAT CHARTS" );

	size_t& cursor = internalState.mCursor;
	SimVal& tech = internalState.mTech;
	SimVal& atk = internalState.mAtk;
	SimDelta& atkField = internalState.mAtkField;
	SimVal& balance = internalState.mBalance;
	SimVal& def = internalState.mDef;
	SimDelta& defField = internalState.mDefField;
	SimColor& color = internalState.mColor;

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		static constexpr size_t kMaxCursorPos = 6;

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
					tech += increase ? 1 : -1;
					break;
				case 1:
					atk += increase ? 1 : -1;
					break;
				case 2:
					atkField += increase ? 1 : -1;
					break;
				case 3:
					balance += increase ? 1 : -1;
					break;
				case 4:
					def += increase ? 1 : -1;
					break;
				case 5:
					defField += increase ? 1 : -1;
					break;
				case 6:
					if( color == SimColor::Unrelated )
					{
						color = SimColor::Same;
					}
					else if( color == SimColor::Same )
					{
						color = SimColor::Clash;
					}
					else
					{
						RF_ASSERT( color == SimColor::Clash );
						color = SimColor::Unrelated;
					}
					break;
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
			drawText( x, y, "*tech  atk  atkField  balance  def  defField  color" );
			break;
		case 1:
			drawText( x, y, " tech *atk  atkField  balance  def  defField  color" );
			break;
		case 2:
			drawText( x, y, " tech  atk *atkField  balance  def  defField  color" );
			break;
		case 3:
			drawText( x, y, " tech  atk  atkField *balance  def  defField  color" );
			break;
		case 4:
			drawText( x, y, " tech  atk  atkField  balance *def  defField  color" );
			break;
		case 5:
			drawText( x, y, " tech  atk  atkField  balance  def *defField  color" );
			break;
		case 6:
			drawText( x, y, " tech  atk  atkField  balance  def  defField *color" );
			break;
		default:
			break;
	}
	y++;
	char const* colorStr = "INVALID";
	switch( color )
	{
		case SimColor::Unrelated:
			colorStr = "UNREL";
			break;
		case SimColor::Same:
			colorStr = "SAME";
			break;
		case SimColor::Clash:
			colorStr = "CLASH";
			break;
		default:
			RF_DBGFAIL();
			break;
	}
	drawText( x, y, " %4i  %3i  %8i  %7i  %3i  %8i  %s", tech, atk, atkField, balance, def, defField, colorStr );

	// Baseline accuracy
	uint8_t const xStart = 1;
	uint8_t const yStart = 5;
	uint8_t const xstep = 7;
	uint8_t const ystep = 1;
	x = xStart;
	y = yStart;
	{
		using Swings = rftl::static_vector<SimVal, 7>;
		using Hits = rftl::static_vector<bool, 7>;
		using Damages = rftl::static_vector<SimVal, 7>;
		using namespace rftl::literals;

		rftl::static_vector<Swings, 11> combos;
		combos.emplace_back( Swings{ 5_u8 } );
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

		// Setup field colors
		FieldColors atkFieldColors;
		{
			SimDelta pool = atkField;
			for( FieldColors::value_type& fieldColor : atkFieldColors )
			{
				if( pool > 0 )
				{
					pool--;
					fieldColor = SimColor::Same;
				}
				else if( pool < 0 )
				{
					pool++;
					fieldColor = SimColor::Clash;
				}
				else
				{
					fieldColor = SimColor::Unrelated;
				}
			}
		}

		for( Swings const& combo : combos )
		{
			Swings const& swings = combo;
			Hits hits;
			Damages damages;

			SimVal comboMeter = 0;
			SimVal counterGuage = 0;

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
					comboMeter = combatEngine.LoCalcNewComboMeter( acc, tech, balance );
				}
				else
				{
					comboMeter = combatEngine.LoCalcContinueComboMeter( comboMeter, acc, tech, balance );
				}
				hit = combatEngine.LoCalcWillAttackHit( comboMeter, balance );
				damage = combatEngine.LoCalcAttackDamage( atk, def, swing, color, atkFieldColors );

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

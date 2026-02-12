#include "stdafx.h"
#include "DevTestCombatCharts.h"

#include "cc3o3/Common.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/combat/Attack.h"
#include "cc3o3/combat/Cast.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/integer_literals.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestCombatCharts::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	size_t mCursor = 0;

	combat::SimVal mTech = 2;
	combat::SimVal mAtk = 2;
	combat::SimDelta mAtkField = 0;
	combat::SimColor mCasterColor = combat::SimColor::Unrelated;
	combat::SimVal mBalance = 2;
	combat::SimVal mDef = 2;
	combat::SimDelta mDefField = 0;
	combat::SimColor mEnemyColor = combat::SimColor::Unrelated;
};

///////////////////////////////////////////////////////////////////////////////

void DevTestCombatCharts::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	//InternalState& internalState = *mInternalState;
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( math::Color3u8::kBlue );
}



void DevTestCombatCharts::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestCombatCharts::OnTick( AppStateTickContext& context )
{
	using namespace combat;

	InternalState& internalState = *mInternalState;

	gfx::ppu::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawText = [&ppu, &font]<typename... TArgs>( uint8_t x, uint8_t y, rftl::format_string<TArgs...> fmt, TArgs... args ) -> bool //
	{
		gfx::ppu::Coord const pos = gfx::ppu::Coord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		bool const retVal = ppu.DebugDrawAuxTextVA( pos, -1, font.mFontHeight, font.mManagedFontID, false, math::Color3u8::kWhite, fmt.get(), rftl::make_format_args( args... ) );
		return retVal;
	};


	drawText( 1, 1, "DEV TEST - COMBAT CHARTS" );

	size_t& cursor = internalState.mCursor;
	SimVal& tech = internalState.mTech;
	SimVal& atk = internalState.mAtk;
	SimDelta& atkField = internalState.mAtkField;
	SimColor& casterColor = internalState.mCasterColor;
	SimVal& balance = internalState.mBalance;
	SimVal& def = internalState.mDef;
	SimDelta& defField = internalState.mDefField;
	SimColor& enemyColor = internalState.mEnemyColor;

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		static constexpr size_t kMaxCursorPos = 7;

		if( focusEvent == ui::focusevent::Command_NavigateLeft )
		{
			cursor--;
		}
		else if( focusEvent == ui::focusevent::Command_NavigateRight )
		{
			cursor++;
		}
		cursor = math::Clamp<size_t>( 0, cursor, kMaxCursorPos );

		static constexpr auto rotateColor = []( SimColor& color ) -> void
		{
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
		};

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
					rotateColor( casterColor );
					break;
				case 4:
					balance += increase ? 1 : -1;
					break;
				case 5:
					def += increase ? 1 : -1;
					break;
				case 6:
					defField += increase ? 1 : -1;
					break;
				case 7:
					rotateColor( enemyColor );
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
			drawText( x, y, "*tech  atk  atkField  cast  balance  def  defField  target" );
			break;
		case 1:
			drawText( x, y, " tech *atk  atkField  cast  balance  def  defField  target" );
			break;
		case 2:
			drawText( x, y, " tech  atk *atkField  cast  balance  def  defField  target" );
			break;
		case 3:
			drawText( x, y, " tech  atk  atkField *cast  balance  def  defField  target" );
			break;
		case 4:
			drawText( x, y, " tech  atk  atkField  cast *balance  def  defField  target" );
			break;
		case 5:
			drawText( x, y, " tech  atk  atkField  cast  balance *def  defField  target" );
			break;
		case 6:
			drawText( x, y, " tech  atk  atkField  cast  balance  def *defField  target" );
			break;
		case 7:
			drawText( x, y, " tech  atk  atkField  cast  balance  def  defField *target" );
			break;
		default:
			break;
	}
	y++;

	static constexpr auto colorStr = []( SimColor color ) -> char const*
	{
		switch( color )
		{
			case SimColor::Unrelated:
				return "UNREL";
			case SimColor::Same:
				return "SAME ";
			case SimColor::Clash:
				return "CLASH";
			default:
				RF_DBGFAIL();
				return "INVLD";
		}
	};
	char const* const casterColorStr = colorStr( casterColor );
	char const* const enemyColorStr = colorStr( enemyColor );
	drawText( x, y, " {:4}  {:3}  {:8}  {} {:7}  {:3}  {:8}  {}", tech, atk, atkField, casterColorStr, balance, def, defField, enemyColorStr );

	// Setup combat instance
	CombatInstance startInstance( gCombatEngine );
	FighterID const attackerID = FighterID::MakeFighter( 0, 0, 0 );
	FighterID const defenderID = FighterID::MakeFighter( 1, 0, 0 );
	{
		Fighter attacker = {};
		Fighter defender = {};

		attacker.mMaxHealth = 1;
		attacker.mCurHealth = attacker.mMaxHealth;
		attacker.mMaxStamina = combat::kMaxStamina;
		attacker.mCurStamina = attacker.mMaxStamina;
		attacker.mMaxCharge = combat::kMaxCharge;
		attacker.mCurCharge = attacker.mMaxCharge;
		attacker.mPhysAtk = atk;
		attacker.mElemAtk = atk;
		attacker.mTechniq = tech;
		attacker.mInnate = element::MakeInnateIdentifier( "red" );
		element::InnateIdentifier const same = attacker.mInnate;
		element::InnateIdentifier const clash = element::MakeInnateIdentifier( "blu" );
		element::InnateIdentifier const unrel = element::MakeInnateIdentifier( "wht" );

		defender.mMaxHealth = kMaxHealth;
		defender.mCurHealth = defender.mMaxHealth;
		defender.mPhysDef = def;
		defender.mElemDef = def;
		defender.mBalance = balance;
		switch( enemyColor )
		{
			case SimColor::Unrelated:
				defender.mInnate = unrel;
				break;
			case SimColor::Same:
				defender.mInnate = same;
				break;
			case SimColor::Clash:
				defender.mInnate = clash;
				break;
			default:
				RF_DBGFAIL();
		}

		startInstance.AddTeam();
		startInstance.AddParty( TeamID::MakeTeam( 0 ) );
		startInstance.AddFighter( PartyID::MakeParty( 0, 0 ) );
		startInstance.SetCombatant( attackerID, attacker );
		startInstance.AddTeam();
		startInstance.AddParty( TeamID::MakeTeam( 1 ) );
		startInstance.AddFighter( PartyID::MakeParty( 1, 0 ) );
		startInstance.SetCombatant( defenderID, defender );

		// Field influence
		startInstance.AddFieldInfluence( unrel, 5 );
		if( atkField < 0 )
		{
			// Unfavorable
			startInstance.AddFieldInfluence( clash, math::integer_cast<size_t>( -atkField ) );
		}
		else
		{
			// Favorable
			startInstance.AddFieldInfluence( same, math::integer_cast<size_t>( atkField ) );
		}
	}

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

		for( Swings const& combo : combos )
		{
			CombatInstance comboInstance = startInstance;

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

				AttackResult const result = comboInstance.ExecuteAttack( attackerID, defenderID, swing );
				comboMeter = result.mNewComboMeter;
				hit = result.mHit;
				counterGuage = comboInstance.GetCounterGuage( defenderID.GetParty() );
				damage = result.mDamage;

				if( hit )
				{
					damageTotal += damage;
					drawText( x, y, " {}:{:2}", swing, damage );
					y += ystep;
				}
				else
				{
					damageTotal += 0;
					drawText( x, y, " {}:--", swing );
					y += ystep;
				}
			}
			drawText( x, y, "======" );
			y += ystep;
			drawText( x, y, "ME:{:3}", comboMeter );
			y += ystep;
			drawText( x, y, "======" );
			y += ystep;
			drawText( x, y, "CG:{:3}", counterGuage );
			y += ystep;
			drawText( x, y, "TO:{:3}", damageTotal );
			y += ystep;
			drawText( x, y, "DS:{:3}", damageTotal / numSwings );
			y += ystep;

			x += xstep;
			y = yStart;
		}
	}

	x = 1;
	y = 20;
	{
		rftl::static_vector<SimVal, 8> casts;
		for( SimVal val = 1; val <= 8; val++ )
		{
			casts.emplace_back( val );
		}
		rftl::static_vector<SimVal, 8> damages;
		rftl::static_vector<SimVal, 8> counterGuages;
		for( SimVal const& cast : casts )
		{
			CombatInstance castInstance = startInstance;
			castInstance.StartCast( attackerID, cast );
			RF_TODO_ANNOTATION( "Configurable casting colors" );
			CastDamageResult const result = castInstance.ApplyCastDamage(
				attackerID,
				defenderID,
				cast,
				cast,
				false,
				element::MakeInnateIdentifier( "red" ) );
			damages.push_back( result.mDamage );
			counterGuages.push_back( result.mCoungerGuageIncrease );
			castInstance.FinishCast( attackerID );
		}
		drawText( x, y, "ELEM DMG {:2} {:2} {:2} {:2} {:2} {:2} {:2} {:2}",
			damages.at( 0 ),
			damages.at( 1 ),
			damages.at( 2 ),
			damages.at( 3 ),
			damages.at( 4 ),
			damages.at( 5 ),
			damages.at( 6 ),
			damages.at( 7 ) );
		y++;
		drawText( x, y, "ELEM CG  {:2} {:2} {:2} {:2} {:2} {:2} {:2} {:2}",
			counterGuages.at( 0 ),
			counterGuages.at( 1 ),
			counterGuages.at( 2 ),
			counterGuages.at( 3 ),
			counterGuages.at( 4 ),
			counterGuages.at( 5 ),
			counterGuages.at( 6 ),
			counterGuages.at( 7 ) );
		y++;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

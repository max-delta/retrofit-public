#include "stdafx.h"
#include "DevTestElementLab.h"

#include "cc3o3/Common.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/casting/CastError.h"
#include "cc3o3/casting/CastingEngine.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/elements/ElementDatabase.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/ui/LocalizationHelpers.h"

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

	size_t mCursor = 0;

	size_t mIndex = 0;
	combat::SimDelta mLevelOffset = 0;

	using ElemResultCache = rftl::unordered_map<element::ElementIdentifier, rftl::optional<bool>, math::DirectHash>;
	ElemResultCache mElemResultCache;
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
	using namespace element;

	InternalState& internalState = *mInternalState;

	gfx::ppu::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawVaColorText = [&ppu, &font]( uint8_t x, uint8_t y, math::Color3f const& color, char const* fmt, va_list args ) -> bool //
	{
		gfx::ppu::Coord const pos = gfx::ppu::Coord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		return ppu.DebugDrawAuxText( pos, -1, font.mFontHeight, font.mManagedFontID, false, color, fmt, args );
	};
	auto const drawColorText = [&drawVaColorText]( uint8_t x, uint8_t y, math::Color3f const& color, char const* fmt, ... ) -> bool //
	{
		va_list args;
		va_start( args, fmt );
		bool const retVal = drawVaColorText( x, y, color, fmt, args );
		va_end( args );
		return retVal;
	};
	auto const drawText = [&drawVaColorText]( uint8_t x, uint8_t y, char const* fmt, ... ) -> bool //
	{
		va_list args;
		va_start( args, fmt );
		bool const retVal = drawVaColorText( x, y, math::Color3f::kWhite, fmt, args );
		va_end( args );
		return retVal;
	};


	drawText( 1, 1, "DEV TEST - ELEMENT LAB" );

	ElementDatabase::ElementIdentifiers const identifiers =
		gElementDatabase->GetAllElementIdentifiers();

	size_t& cursor = internalState.mCursor;
	size_t& index = internalState.mIndex;
	SimDelta& levelOffset = internalState.mLevelOffset;

	InternalState::ElemResultCache& elemResultCache = internalState.mElemResultCache;

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		static constexpr size_t kMaxCursorPos = 1;

		if( focusEvent == ui::focusevent::Command_NavigateLeft )
		{
			cursor--;
		}
		else if( focusEvent == ui::focusevent::Command_NavigateRight )
		{
			cursor++;
		}
		cursor = math::Clamp<size_t>( 0, cursor, kMaxCursorPos );

		// Special case for index, since it has a large range
		if( cursor == 0 )
		{
			switch( focusEvent )
			{
				case ui::focusevent::Command_NavigateToNextGroup:
					index += 10;
					break;
				case ui::focusevent::Command_NavigateToPreviousGroup:
					index -= 10;
					break;
				case ui::focusevent::Command_NavigateToFirst:
					index = 0;
					break;
				case ui::focusevent::Command_NavigateToLast:
					index = identifiers.size() - 1;
					break;
				default:
					break;
			}
		}

		bool increase = focusEvent == ui::focusevent::Command_NavigateUp;
		bool decrease = focusEvent == ui::focusevent::Command_NavigateDown;
		if( increase || decrease )
		{
			RF_ASSERT( increase != decrease );
			switch( cursor )
			{
				case 0:
					// NOTE: Inverted list, down increases
					{
						int64_t const tentative = math::integer_cast<int64_t>( index ) + ( increase ? -1 : 1 );
						index =
							tentative >= 0 ?
							math::integer_cast<size_t>( tentative ) :
							rftl::numeric_limits<size_t>::max();
					}
					break;
				case 1:
					levelOffset += increase ? 1 : -1;
					break;
				default:
					break;
			}
		}
	}

	// Select the element to cast from the list of all available
	if( index == identifiers.size() )
	{
		// At end of list, rollover by only one (assume forward past end)
		index = 0;
	}
	else if( index > identifiers.size() )
	{
		// Rollover by a lot (assume backwards past front)
		index = identifiers.size() - 1;
	}
	index = math::Clamp<size_t>( 0, index, identifiers.size() - 1 );
	ElementIdentifier const elementToCast = identifiers.at( index );

	// Set the casted level to the base level, adjusted by optional offset
	// NOTE: Multiple clamps, for both user error and data error
	ElementDesc const desc = gElementDatabase->GetElementDesc( elementToCast );
	ElementLevel const castedLevel =
		math::Clamp<ElementLevel>(
			kMinElementLevel,
			math::Clamp<ElementLevel>(
				desc.mMinLevel,
				math::integer_truncast<ElementLevel>(
					math::integer_truncast<int64_t>( desc.mBaseLevel ) + levelOffset ),
				desc.mMaxLevel ),
			kMaxElementLevel );

	// Setup initial combat instance
	CombatInstance startInstance( gCombatEngine );
	FighterID attackerID = {};
	FighterID defenderID = {};
	{
		Fighter base = {};
		{
			static constexpr SimVal kBaselineStat = 2;

			CombatEngine const& combatEngine = *gCombatEngine;

			// TODO: Configuration of innate?
			base.mEntityClass = EntityClass::Hero;
			base.mInnate = MakeInnateIdentifier( "red" );
			base.mMaxHealth = combatEngine.LoCalcMaxHealth( kBaselineStat, base.mEntityClass );
			base.mCurHealth = base.mMaxHealth;
			base.mMaxStamina = combat::kMaxStamina;
			base.mCurStamina = base.mMaxStamina;
			base.mMaxCharge = combat::kMaxCharge;
			base.mCurCharge = base.mMaxCharge;
			base.mPhysAtk = kBaselineStat;
			base.mPhysDef = kBaselineStat;
			base.mTechniq = kBaselineStat;
			base.mElemAtk = kBaselineStat;
			base.mElemDef = kBaselineStat;
			base.mBalance = kBaselineStat;
			base.mTechniq = kBaselineStat;
		}

		Fighter attacker = base;
		Fighter attackerAlly = attacker;

		Fighter defender = base;
		InnateIdentifier const same = attacker.mInnate;
		InnateIdentifier const clash = MakeInnateIdentifier( "blu" );
		InnateIdentifier const unrel = MakeInnateIdentifier( "wht" );
		// TODO: Configuration of innate?
		SimColor const color = SimColor::Unrelated;
		switch( color )
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

		Fighter defenderAlly = defender;

		// Attacker team
		{
			TeamID const teamID = startInstance.AddTeam();
			PartyID const partyID = startInstance.AddParty( teamID );
			attackerID = startInstance.AddFighter( partyID );
			startInstance.SetCombatant( attackerID, attacker );
			FighterID const allyID = startInstance.AddFighter( partyID );
			startInstance.SetCombatant( allyID, attackerAlly );
		}

		// Defender team
		{
			TeamID const teamID = startInstance.AddTeam();
			PartyID const partyID = startInstance.AddParty( teamID );
			defenderID = startInstance.AddFighter( partyID );
			startInstance.SetCombatant( defenderID, attacker );
			FighterID const allyID = startInstance.AddFighter( partyID );
			startInstance.SetCombatant( allyID, defenderAlly );
		}

		// Field influence
		startInstance.AddFieldInfluence( unrel, 5 );
		SimDelta const atkField = 0;
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

	// Cast the element to create a new combat instance
	CombatInstance resultInstance = startInstance;
	UniquePtr<cast::CastError> const castError = gCastingEngine->ExecuteElementCast(
		resultInstance,
		attackerID,
		defenderID,
		elementToCast,
		castedLevel );

	// Update cache
	if( castError == cast::CastError::kNoError )
	{
		elemResultCache[elementToCast] = true;
	}
	else
	{
		elemResultCache[elementToCast] = false;
	}

	uint8_t x = 0;
	uint8_t y = 0;

	auto const drawIdentifiers = [&x, &y, &drawColorText, &identifiers, &elemResultCache, index]() -> void
	{
		static constexpr size_t kSpread = 8;
		bool const upperFit = index >= kSpread;
		bool const LowerFit = index + kSpread < identifiers.size();
		size_t const upper = upperFit ? index - kSpread : 0;
		size_t const upperPad = upperFit ? 0 : kSpread - ( index - upper );
		size_t const lower = LowerFit ? index + kSpread : identifiers.size() - 1;
		size_t const lowerPad = LowerFit ? 0 : kSpread - ( lower - index );
		for( size_t i = 0; i < upperPad; i++ )
		{
			drawColorText( x, y, math::Color3f::kGray75, " . . ." );
			y++;
		}
		for( size_t i = upper; i <= lower; i++ )
		{
			ElementIdentifier const identifier = identifiers.at( i );

			// Color based on cached result
			rftl::optional<bool> const& result = elemResultCache[identifier];
			math::Color3f color = math::Color3f::kWhite;
			if( result.has_value() )
			{
				color = result.value() ? math::Color3f::kGreen : math::Color3f::kRed;
			}

			drawColorText( x, y, color, "%c%s",
				i == index ? '*' : ' ',
				GetElementString( identifier ).c_str() );
			y++;
		}
		for( size_t i = 0; i < lowerPad; i++ )
		{
			drawColorText( x, y, math::Color3f::kGray75, " . . ." );
			y++;
		}
	};

	auto const drawInstance = [&x, &y, &drawText, attackerID, defenderID]( CombatInstance const& instance ) -> void
	{
		CombatEngine const& combatEngine = *gCombatEngine;

		// Field
		{
			Field::Influence influence = instance.GetFieldInfluence();
			rftl::static_string<influence.max_size()> fieldGlyphs = {};
			for( InnateIdentifier const& innate : influence )
			{
				fieldGlyphs.push_back( combatEngine.DisplayInnateGlyphAscii( innate ) );
			}
			drawText( x, y, "Field %s", fieldGlyphs.c_str() );
			y++;
		}

		// Party guages
		combat::CombatInstance::PartyIDs const partyIDs = instance.GetPartyIDs();
		for( combat::PartyID const& partyID : partyIDs )
		{
			combat::Party const party = instance.GetParty( partyID );
			drawText( x, y,
				"t%i:p%i  Guage %03i/%03i",
				partyID.GetTeamIndex(),
				partyID.GetPartyIndex(),
				party.mCounterGuage,
				combat::kCounterGaugeFull );
			y++;
		}

		combat::CombatInstance::FighterIDs const fighterIDs = instance.GetFighterIDs();
		for( combat::FighterID const& fighterID : fighterIDs )
		{
			combat::Fighter const fighter = instance.GetFighter( fighterID );
			char marker = ' ';
			if( fighterID == attackerID )
			{
				marker = '*';
			}
			else if( fighterID == defenderID )
			{
				marker = 'X';
			}
			drawText( x, y,
				"%c%i:%i:%i",
				marker,
				fighterID.GetTeamIndex(),
				fighterID.GetPartyIndex(),
				fighterID.GetFighterIndex() );
			y++;
			drawText( x, y,
				"    H%03i/%03i S%1i/%1i C%01i/%01i",
				fighter.mCurHealth,
				fighter.mMaxHealth,
				fighter.mCurStamina,
				fighter.mMaxStamina,
				fighter.mCurCharge,
				fighter.mMaxCharge );
			y++;
			drawText( x, y,
				"    Pa%id%i Ea%id%i B%iT%i",
				fighter.mPhysAtk,
				fighter.mPhysDef,
				fighter.mElemAtk,
				fighter.mElemDef,
				fighter.mBalance,
				fighter.mTechniq );
			y++;
			drawText( x, y,
				"    Combo %2i -> %i:%i:%i",
				fighter.mComboMeter,
				fighter.mComboTarget.GetTeamIndex(),
				fighter.mComboTarget.GetPartyIndex(),
				fighter.mComboTarget.GetFighterIndex() );
			y++;
		}
	};

	static constexpr uint8_t kInstance_y = 5;
	static constexpr uint8_t kInstanceA_x = 25;
	static constexpr uint8_t kInstanceB_x = 55;
	static constexpr uint8_t kDesc_x = 15;
	static constexpr uint8_t kDesc_y = 25;

	x = 2;
	y = 2;

	switch( cursor )
	{
		case 0:
			drawText( x, y, "*index  offset" );
			break;
		case 1:
			drawText( x, y, " index *offset" );
			break;
		default:
			break;
	}
	y++;
	drawText( x, y, " %5i  %3i", index, levelOffset );
	y++;

	// Cast error, if present
	x = kInstanceA_x;
	drawColorText( x, y, math::Color3f::kCyan, "CAST PREP" );
	x = kInstanceB_x;
	if( castError == cast::CastError::kNoError )
	{
		drawColorText( x, y, math::Color3f::kGreen, "CAST SUCCESS" );
	}
	else
	{
		drawColorText( x, y, math::Color3f::kRed, "CAST ERROR" );
	}
	y++;

	x = 1;
	y = kInstance_y;
	drawIdentifiers();

	x = kInstanceA_x;
	y = kInstance_y;
	drawInstance( startInstance );
	x = kInstanceB_x;
	y = kInstance_y;
	drawInstance( resultInstance );

	x = kDesc_x;
	y = kDesc_y;
	drawColorText( x, y, math::Color3f::kCyan, "%s  @lvl %u  [%u-%u-%u]  '%s'",
		GetInnateString( desc.mInnate ).c_str(),
		castedLevel,
		desc.mMinLevel,
		desc.mBaseLevel,
		desc.mMaxLevel,
		ui::LocalizeKey( GetElementName( elementToCast ) ).c_str() );
	y++;
	drawColorText( x, y, math::Color3f::kCyan, "'%s'",
		ui::LocalizeKey( GetElementSynopsis( elementToCast ) ).c_str() );
	y++;
}

///////////////////////////////////////////////////////////////////////////////
}

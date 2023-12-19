#include "stdafx.h"
#include "CombatEngine.h"

#include "cc3o3/combat/Attack.h"
#include "cc3o3/combat/Cast.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"
#include "core_math/Rand.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

CombatEngine::CombatEngine( WeakPtr<file::VFS const> const& vfs )
	: mVfs( vfs )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

SimColor CombatEngine::EvalInnates( element::InnateIdentifier lhs, element::InnateIdentifier rhs ) const
{
	if( lhs == rhs )
	{
		return SimColor::Same;
	}

	// TODO: Use data from a file instead
	using Pair = rftl::pair<element::InnateString, element::InnateString>;
	using I = element::InnateString;
	rftl::array<Pair, 3> const pairs = {
		Pair{ I( "wht" ), I( "blk" ) },
		Pair{ I( "red" ), I( "blu" ) },
		Pair{ I( "yel" ), I( "grn" ) },
	};

	for( Pair const& pair : pairs )
	{
		if( lhs == pair.first && rhs == pair.second )
		{
			return SimColor::Clash;
		}
		if( lhs == pair.second && rhs == pair.first )
		{
			return SimColor::Clash;
		}
	}

	return SimColor::Unrelated;
}



element::InnateIdentifier CombatEngine::GenerateRandomInnate( uint32_t& lastGeneratorValue ) const
{
	// TODO: Use data from a file instead
	using I = element::InnateString;
	rftl::array<I, 6> const innates = {
		I( "wht" ),
		I( "blk" ),
		I( "red" ),
		I( "blu" ),
		I( "yel" ),
		I( "grn" ),
	};
	lastGeneratorValue = math::StableRandLCG( lastGeneratorValue );
	return element::MakeInnateIdentifier( innates.at( lastGeneratorValue % innates.size() ) );
}

///////////////////////////////////////////////////////////////////////////////

DisplayVal CombatEngine::DisplayHealth( LargeSimVal healthVal, EntityClass entityClass ) const
{
	return healthVal;
}



SignedDisplayVal CombatEngine::DisplayStamina( SimDelta staminaVal, EntityClass entityClass ) const
{
	return staminaVal;
}



DisplayVal CombatEngine::DisplayCharge( SimVal chargeVal, EntityClass entityClass ) const
{
	return chargeVal;
}



DisplayVal CombatEngine::DisplayStandardStat( SimVal statVal, EntityClass entityClass ) const
{
	static constexpr SimVal kMinStat = 0;
	static constexpr SimVal kMaxStat = 10;
	statVal = math::Clamp( kMinStat, statVal, kMaxStat );

	switch( entityClass )
	{
		case EntityClass::Peasant:
		{
			static constexpr DisplayVal kBaseline = 5;
			static constexpr SimVal kScalar = 2;
			static_assert( kBaseline + kMaxStat * kScalar < rftl::numeric_limits<DisplayVal>::max() );
			return math::integer_cast<DisplayVal>( kBaseline + statVal * kScalar );
		}
		case EntityClass::Hero:
		case EntityClass::Monster:
		{
			static constexpr DisplayVal kBaseline = 10;
			static constexpr SimVal kScalar = 3;
			static_assert( kBaseline + kMaxStat * kScalar < rftl::numeric_limits<DisplayVal>::max() );
			return math::integer_cast<DisplayVal>( kBaseline + statVal * kScalar );
		}
		case EntityClass::Invalid:
		default:
		{
			RF_DBGFAIL();
			return 0;
		}
	}
}



char CombatEngine::DisplayInnateGlyphAscii( element::InnateIdentifier innate ) const
{
	// TODO: Use data from a file instead
	using I = element::InnateString;
	rftl::array<I, 6> const innates = {
		I( "wht" ),
		I( "blk" ),
		I( "red" ),
		I( "blu" ),
		I( "yel" ),
		I( "grn" ),
	};
	rftl::array<char, 6> const glyphs = {
		'W',
		'B',
		'R',
		'U',
		'Y',
		'G',
	};
	for( size_t i = 0; i < 6; i++ )
	{
		if( innate == innates.at( i ) )
		{
			return glyphs.at( i );
		}
	}
	return '?';
}

///////////////////////////////////////////////////////////////////////////////

AttackResult CombatEngine::HiCalcAttack( AttackProfile const& profile ) const
{
	AttackResult retVal = {};

	SimVal const attackAccuracy = LoCalcAttackAccuracy( profile.mAttackStrength );

	if( profile.mNewCombo )
	{
		retVal.mNewComboMeter = LoCalcNewComboMeter(
			attackAccuracy,
			profile.mAttackerTechnique,
			profile.mDefenderBalance );
	}
	else
	{
		retVal.mNewComboMeter = LoCalcContinueComboMeter(
			profile.mComboMeter,
			attackAccuracy,
			profile.mAttackerTechnique,
			profile.mDefenderBalance );
	}

	retVal.mHit = LoCalcWillAttackHit(
		retVal.mNewComboMeter,
		profile.mDefenderBalance );

	if( retVal.mHit )
	{
		SimColor const combatantClash = EvalInnates(
			profile.mAttackerInnate,
			profile.mDefenderInnate );

		FieldColors attackerInfluenced = {};
		for( size_t i = 0; i < kFieldSize; i++ )
		{
			attackerInfluenced.at( i ) = EvalInnates(
				profile.mAttackerInnate,
				profile.mInfluence.at( i ) );
		}

		retVal.mDamage = LoCalcAttackDamage(
			profile.mAttackerPhysicalAttack,
			profile.mDefenderPhysicalDefense,
			profile.mAttackStrength,
			combatantClash,
			attackerInfluenced );

		retVal.mCoungerGuageIncrease = 0;
		retVal.mCoungerGuageIncrease += LoCalcCounterFromAttackSwing( retVal.mNewComboMeter );
		retVal.mCoungerGuageIncrease += LoCalcCounterFromAttackDamage( retVal.mDamage );
	}
	else
	{
		retVal.mDamage = 0;
		retVal.mCoungerGuageIncrease = LoCalcCounterFromAttackSwing( retVal.mNewComboMeter );
	}

	return retVal;
}



CastDamageResult CombatEngine::HiCalcCast( CastDamageProfile const& profile ) const
{
	CastDamageResult retVal = {};

	SimColor const casterClash = EvalInnates(
		profile.mElementInnate,
		profile.mAttackerInnate );
	SimColor const defenderClash = EvalInnates(
		profile.mElementInnate,
		profile.mDefenderInnate );

	FieldColors elementInfluenced = {};
	for( size_t i = 0; i < kFieldSize; i++ )
	{
		elementInfluenced.at( i ) = EvalInnates(
			profile.mElementInnate,
			profile.mInfluence.at( i ) );
	}

	retVal.mDamage = LoCalcElementDamage(
		profile.mAttackerElementalAttack,
		profile.mDefenderElementalDefense,
		profile.mElementStrength,
		profile.mCastedLevel,
		profile.mMultiTarget,
		casterClash,
		defenderClash,
		elementInfluenced );

	retVal.mCoungerGuageIncrease = 0;
	retVal.mCoungerGuageIncrease += LoCalcCounterFromElementDamage( retVal.mDamage );

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

LargeSimVal CombatEngine::LoCalcMaxHealth( SimVal healthStat, EntityClass entityClass ) const
{
	static constexpr SimVal kMinStat = 0;
	static constexpr SimVal kMaxStat = 15;
	healthStat = math::Clamp( kMinStat, healthStat, kMaxStat );

	switch( entityClass )
	{
		case EntityClass::Peasant:
		{
			static constexpr LargeSimVal kBaseline = 70;
			static constexpr SimVal kScalar = 5;
			static_assert( kBaseline + kMaxStat * kScalar < rftl::numeric_limits<LargeSimVal>::max() );
			return math::integer_cast<LargeSimVal>( kBaseline + healthStat * kScalar );
		}
		case EntityClass::Hero:
		{
			static constexpr LargeSimVal kBaseline = 80;
			static constexpr SimVal kScalar = 10;
			static_assert( kBaseline + kMaxStat * kScalar < rftl::numeric_limits<LargeSimVal>::max() );
			return math::integer_cast<LargeSimVal>( kBaseline + healthStat * kScalar );
		}
		case EntityClass::Monster:
		{
			static constexpr LargeSimVal kBaseline = 100;
			static constexpr SimVal kScalar = 50;
			static_assert( kBaseline + kMaxStat * kScalar < rftl::numeric_limits<LargeSimVal>::max() );
			return math::integer_cast<LargeSimVal>( kBaseline + healthStat * kScalar );
		}
		case EntityClass::Invalid:
		default:
		{
			RF_DBGFAIL();
			return 0;
		}
	}
}



SimVal CombatEngine::LoCalcIdleStaminaGainAlliedTurn() const
{
	// TODO: This should be re-tuned so allies are suppressed
	return 1;
}



SimVal CombatEngine::LoCalcIdleStaminaGainOpposingTurn() const
{
	// TODO: This should be re-tuned so allies are suppressed
	return 1;
}



BreakClass CombatEngine::LoCalcAttackBreakClass( SimVal attackerPhysAtkStat, SimVal defenderPhysDefStat ) const
{
	static constexpr SimVal kBreakPoint = kMaxBreakDelta / 2;
	static_assert( kBreakPoint * 2 == kMaxBreakDelta, "Break not cleanly divisible" );
	if( attackerPhysAtkStat > defenderPhysDefStat + kBreakPoint * 2 )
	{
		// Unreasonably stronger (>2xbreak)
		return BreakClass::Overwhelming;
	}
	else if( attackerPhysAtkStat > defenderPhysDefStat + kBreakPoint )
	{
		// Crazy stronger (>break)
		return BreakClass::Critical;
	}
	else if( attackerPhysAtkStat >= defenderPhysDefStat )
	{
		// Equal or stronger (>)
		return BreakClass::Normal;
	}
	else if( attackerPhysAtkStat + kBreakPoint >= defenderPhysDefStat )
	{
		// Weaker, but not terribly so (<)
		return BreakClass::Weak;
	}
	else
	{
		// Heavily outclassed (<break)
		return BreakClass::Ineffective;
	}
}



BreakClass CombatEngine::LoCalcElementBreakClass( SimVal attackerElemAtkStat, SimVal defenderElemDefStat ) const
{
	// For now, use same logic as attacks
	return LoCalcAttackBreakClass( attackerElemAtkStat, defenderElemDefStat );
}



SimVal CombatEngine::LoCalcAttackerBonus( BreakClass breakClass, SimVal attackerPhysAtkStat, SimVal defenderPhysDefStat ) const
{
	static_assert( kMaxAttackerBonus == 15, "Check balance" );
	static constexpr SimVal kAttackBaseBonus = 5;
	static_assert( kMaxAttackerBonus == kMaxBreakDelta + kAttackBaseBonus, "Check balance" );

	SimVal attackerBonus = 0;
	switch( breakClass )
	{
		case BreakClass::Overwhelming:
			// Max out the damage
			attackerBonus = kMaxAttackerBonus;
			break;
		case BreakClass::Critical:
			// Same as normal, rely on weapon to perform crit
			attackerBonus = 0u + kAttackBaseBonus + ( attackerPhysAtkStat - defenderPhysDefStat );
			break;
		case BreakClass::Normal:
			// Base bonus, plus attacker skill
			RF_ASSERT( attackerPhysAtkStat >= defenderPhysDefStat );
			attackerBonus = 0u + kAttackBaseBonus + ( attackerPhysAtkStat - defenderPhysDefStat );
			break;
		case BreakClass::Weak:
			// Base bonus, minus defender skill
			RF_ASSERT( attackerPhysAtkStat <= defenderPhysDefStat );
			attackerBonus = 0u + ( attackerPhysAtkStat + kAttackBaseBonus ) - defenderPhysDefStat;
			break;
		case BreakClass::Ineffective:
			// No damage, rely on weapon
			attackerBonus = 0;
			break;
		default:
			RF_DBGFAIL_MSG( "Unexpected codepath" );
			break;
	}

	RF_ASSERT( attackerBonus <= kMaxAttackerBonus );
	attackerBonus = math::Clamp<SimVal>( 0u, attackerBonus, kMaxAttackerBonus );
	return attackerBonus;
}



SimVal CombatEngine::LoCalcCasterBonus( BreakClass breakClass, SimVal attackerElemAtkStat, SimVal defenderElemDefStat ) const
{
	static_assert( kMaxCasterBonus == 15, "Check balance" );
	static constexpr SimVal kCastBaseBonus = 5;
	static_assert( kMaxCasterBonus == kMaxBreakDelta + kCastBaseBonus, "Check balance" );

	SimVal casterBonus = 0;
	switch( breakClass )
	{
		case BreakClass::Overwhelming:
			// Max out the damage
			casterBonus = kMaxCasterBonus;
			break;
		case BreakClass::Critical:
			// Same as normal, rely on element to perform crit
			casterBonus = 0u + kCastBaseBonus + ( attackerElemAtkStat - defenderElemDefStat );
			break;
		case BreakClass::Normal:
			// Base bonus, plus attacker skill
			RF_ASSERT( attackerElemAtkStat >= defenderElemDefStat );
			casterBonus = 0u + kCastBaseBonus + ( attackerElemAtkStat - defenderElemDefStat );
			break;
		case BreakClass::Weak:
			// Base bonus, minus defender skill
			RF_ASSERT( attackerElemAtkStat <= defenderElemDefStat );
			casterBonus = 0u + ( attackerElemAtkStat + kCastBaseBonus ) - defenderElemDefStat;
			break;
		case BreakClass::Ineffective:
			// No damage, rely on element
			casterBonus = 0;
			break;
		default:
			RF_DBGFAIL_MSG( "Unexpected codepath" );
			break;
	}

	RF_ASSERT( casterBonus <= kMaxAttackerBonus );
	casterBonus = math::Clamp<SimVal>( 0u, casterBonus, kMaxAttackerBonus );
	return casterBonus;
}



SimVal CombatEngine::LoCalcWeaponBonus( BreakClass breakClass, SimVal attackStrength ) const
{
	static_assert( kMaxWeaponBonus == 20, "Check balance" );
	static_assert( kMaxAttackStrength == 5, "Check balance" );
	static constexpr SimVal kCriticalWeaponModifier = 4;
	static constexpr SimVal kNormalWeaponModifier = 3;
	static constexpr SimVal kWeakWeaponModifier = 2;
	static constexpr SimVal kIneffectiveWeaponModifier = 1;

	SimVal weaponBonus = 0;
	switch( breakClass )
	{
		case BreakClass::Overwhelming:
			// Same as critical, rely on attacker to push limits
			weaponBonus = 0u + attackStrength * kCriticalWeaponModifier;
			break;
		case BreakClass::Critical:
			weaponBonus = 0u + attackStrength * kCriticalWeaponModifier;
			break;
		case BreakClass::Normal:
			weaponBonus = 0u + attackStrength * kNormalWeaponModifier;
			break;
		case BreakClass::Weak:
			weaponBonus = 0u + attackStrength * kWeakWeaponModifier;
			break;
		case BreakClass::Ineffective:
			weaponBonus = 0u + attackStrength * kIneffectiveWeaponModifier;
			break;
		default:
			RF_DBGFAIL_MSG( "Unexpected codepath" );
			break;
	}

	RF_ASSERT( weaponBonus <= kMaxWeaponBonus );
	weaponBonus = math::Clamp<SimVal>( 0u, weaponBonus, kMaxWeaponBonus );
	return weaponBonus;
}



SimVal CombatEngine::LoCalcElementBonus( BreakClass breakClass, SimVal elementStrength, SimVal castedLevel, bool multiTarget ) const
{
	static_assert( kMaxElementBonus == 80, "Check balance" );
	static constexpr SimVal kCriticalElementModifier = 4;
	static constexpr SimVal kNormalElementModifier = 3;
	static constexpr SimVal kWeakElementModifier = 2;
	static constexpr SimVal kIneffectiveElementModifier = 1;

	SimVal elementBonus = 0;

	// Stronger elements are quadratic, so stronger is almost always better
	static_assert( kMaxElementStrength == 8, "Check balance" );
	RF_ASSERT( elementStrength > 0 );
	RF_ASSERT( elementStrength <= kMaxElementStrength );
	// DMG = (LVL / 2) + (LVL * LVL)
	static constexpr rftl::array<SimVal, 8> kSingleBaselines =
		{ 2, 5, 10, 18, 26, 39, 52, 68 };
	// DMG = -10 + (LVL * 5) + (LVL * LVL / 3)
	static constexpr rftl::array<SimVal, 8> kMultiBaselines =
		{ 0, 0, 6, 15, 20, 30, 40, 50 };
	if( multiTarget )
	{
		elementBonus += kMultiBaselines.at( math::integer_cast<size_t>( elementStrength - 1 ) );
	}
	else
	{
		elementBonus += kSingleBaselines.at( math::integer_cast<size_t>( elementStrength - 1 ) );
	}

	// Casted level is linear, so upleveling should usually be avoided
	static_assert( element::kMinElementLevel == 1, "Check balance" );
	static_assert( element::kMaxElementLevel == 8, "Check balance" );
	RF_ASSERT( castedLevel >= element::kMinElementLevel );
	RF_ASSERT( castedLevel <= element::kMaxElementLevel );
	// DMG = LVL
	static constexpr rftl::array<SimVal, 8> kCastedLevels =
		{ 1, 2, 3, 4, 5, 6, 7, 8 };
	elementBonus += kCastedLevels.at( math::integer_cast<size_t>( castedLevel - 1 ) );

	switch( breakClass )
	{
		case BreakClass::Overwhelming:
			// Same as critical, rely on caster to push limits
			elementBonus += kCriticalElementModifier;
			break;
		case BreakClass::Critical:
			elementBonus += kCriticalElementModifier;
			break;
		case BreakClass::Normal:
			elementBonus += kNormalElementModifier;
			break;
		case BreakClass::Weak:
			elementBonus += kWeakElementModifier;
			break;
		case BreakClass::Ineffective:
			elementBonus += kIneffectiveElementModifier;
			break;
		default:
			RF_DBGFAIL_MSG( "Unexpected codepath" );
			break;
	}

	RF_ASSERT( elementBonus <= kMaxElementBonus );
	elementBonus = math::Clamp<SimVal>( 0u, elementBonus, kMaxElementBonus );
	return elementBonus;
}



SimDelta CombatEngine::LoCalcAttackFieldModifier( SimColor attackVsTarget, FieldColors const& attackerField ) const
{
	static_assert( kMaxFieldModifierOffset == 10, "Check balance" );
	static_assert( kFieldSize == 5, "Check balance" );
	static constexpr SimVal kClashBonus = kFieldSize;

	static constexpr SimVal kUnrelatedBonus = 1;
	SimDelta fieldModifier = 0;
	for( SimColor const& atk : attackerField )
	{
		switch( atk )
		{
			case SimColor::Unrelated:
			{
				// No effect
				break;
			}
			case SimColor::Same:
			{
				// In favor
				if( attackVsTarget == SimColor::Same )
				{
					// Negated by defender
				}
				else
				{
					// Bonus
					fieldModifier++;
				}
				break;
			}
			case SimColor::Clash:
			{
				// Penalty
				fieldModifier--;
				break;
			}
			default:
				RF_DBGFAIL();
				break;
		}
	}
	switch( attackVsTarget )
	{
		case SimColor::Unrelated:
		{
			// Favor attacker
			fieldModifier += kUnrelatedBonus;
			break;
		}
		case SimColor::Same:
		{
			// No effect
			break;
		}
		case SimColor::Clash:
		{
			// Heavily favor attacker
			fieldModifier += kClashBonus;
			break;
		}
		default:
			RF_DBGFAIL();
			break;
	}

	RF_ASSERT( fieldModifier >= -kMaxFieldModifierOffset && fieldModifier <= kMaxFieldModifierOffset );
	fieldModifier = math::Clamp<SimDelta>( -kMaxFieldModifierOffset, fieldModifier, kMaxFieldModifierOffset );
	return fieldModifier;
}



SimDelta CombatEngine::LoCalcElementFieldModifier( SimColor elementVsTarget, FieldColors const& elementField ) const
{
	// Use same logic as for attacks
	RF_TODO_ANNOTATION( "Invert this so attacks call this instead, since that makes more sense" );
	return LoCalcAttackFieldModifier( elementVsTarget, elementField );
}



SimDelta CombatEngine::LoCalcCasterAffinityModifier( SimColor elementVsCaster ) const
{
	static_assert( kMaxAffinityModifierOffset == 2, "Check balance" );
	switch( elementVsCaster )
	{
		case SimColor::Unrelated:
		{
			// No effect
			return 0;
		}
		case SimColor::Same:
		{
			// Favor caster
			return math::integer_cast<SimDelta>( kMaxAffinityModifierOffset );
		}
		case SimColor::Clash:
		{
			// Punish caster
			return math::integer_cast<SimDelta>( -kMaxAffinityModifierOffset );
		}
		default:
			RF_DBGFAIL();
			return 0;
	}
}



SimVal CombatEngine::LoCalcAttackStaminaCost( SimVal attackStrength ) const
{
	RF_ASSERT( attackStrength > 0 );
	return attackStrength;
}



SimVal CombatEngine::LoCalcElementStaminaCost() const
{
	static_assert( kMaxStamina == 7, "Check balance" );
	return kMaxStamina;
}



SimVal CombatEngine::LoCalcAttackChargeGain( SimVal attackStrength ) const
{
	RF_ASSERT( attackStrength > 0 );
	return attackStrength;
}



SimVal CombatEngine::LoCalcAttackAccuracy( SimVal attackStrength ) const
{
	if( attackStrength >= 3 )
	{
		// Wild attack, must rely on techniq alone
		return 0;
	}
	else if( attackStrength == 2 )
	{
		// Normal attack
		return 1;
	}
	else if( attackStrength == 1 )
	{
		// Precise attack
		return 2;
	}
	else
	{
		// Useless attack
		RF_ASSERT( attackStrength <= 0 );
		return 0;
	}
}



SimVal CombatEngine::LoCalcNewComboMeter(
	SimVal attackAccuracy,
	SimVal attackerTechniqStat,
	SimVal defenderBalanceStat ) const
{
	static constexpr SimVal kMaxTechniq = 10u;
	attackerTechniqStat = math::Clamp<SimVal>( 0u, attackerTechniqStat, kMaxTechniq );

	SimVal const minNeeded = LoCalcMinComboToHit( defenderBalanceStat );
	SimVal const maxMeter = 0u + minNeeded + attackAccuracy;
	SimVal const unchecked = 0u + attackAccuracy + attackerTechniqStat;

	if( unchecked >= minNeeded )
	{
		RF_ASSERT( LoCalcWillAttackHit( unchecked, defenderBalanceStat ) );
		return maxMeter;
	}
	else
	{
		return unchecked;
	}
}



SimVal CombatEngine::LoCalcContinueComboMeter(
	SimVal attackerComboMeter,
	SimVal attackAccuracy,
	SimVal attackerTechniqStat,
	SimVal defenderBalanceStat ) const
{
	static constexpr SimVal kMaxTechniq = 10u;
	attackerTechniqStat = math::Clamp<SimVal>( 0u, attackerTechniqStat, kMaxTechniq );

	SimVal const minNeeded = LoCalcMinComboToHit( defenderBalanceStat );
	SimVal const maxMeter = 0u + minNeeded + attackAccuracy;
	SimVal const unchecked = 0u + attackerComboMeter + attackAccuracy + attackerTechniqStat;

	if( unchecked >= minNeeded )
	{
		RF_ASSERT( LoCalcWillAttackHit( unchecked, defenderBalanceStat ) );
		return maxMeter;
	}
	else
	{
		return unchecked;
	}
}



SimVal CombatEngine::LoCalcMinComboToHit( SimVal defenderBalanceStat ) const
{
	static constexpr SimVal kMaxBalance = 10u;
	defenderBalanceStat = math::Clamp<SimVal>( 0u, defenderBalanceStat, kMaxBalance );

	if( defenderBalanceStat <= 0 )
	{
		// No balance, everything hits
		return 0;
	}
	else if( defenderBalanceStat <= 2 )
	{
		// Low/normal balance
		return defenderBalanceStat * 2u;
	}
	else
	{
		// Strong balance
		return defenderBalanceStat + 2u;
	}
}



bool CombatEngine::LoCalcWillAttackHit( SimVal attackerComboMeter, SimVal defenderBalanceStat ) const
{
	return attackerComboMeter >= LoCalcMinComboToHit( defenderBalanceStat );
}



SimVal CombatEngine::LoCalcAttackDamage(
	SimVal attackerPhysAtkStat,
	SimVal defenderPhysDefStat,
	SimVal attackStrength,
	SimColor attackVsTarget,
	FieldColors const& attackerField ) const
{
	RF_ASSERT( attackStrength > 0 );
	RF_ASSERT( attackStrength <= kMaxAttackStrength );
	attackStrength = math::Clamp<SimVal>( 0u, attackStrength, kMaxAttackStrength );

	// Physical portion
	BreakClass const breakClass = LoCalcAttackBreakClass( attackerPhysAtkStat, defenderPhysDefStat );
	SimVal const attackerBonus = LoCalcAttackerBonus( breakClass, attackerPhysAtkStat, defenderPhysDefStat );
	SimVal const weaponBonus = LoCalcWeaponBonus( breakClass, attackStrength );

	// Elemental portion
	SimDelta const fieldModifier = LoCalcAttackFieldModifier( attackVsTarget, attackerField );

	RF_ASSERT( attackerBonus <= kMaxAttackerBonus );
	RF_ASSERT( weaponBonus <= kMaxWeaponBonus );
	RF_ASSERT( fieldModifier >= -kMaxFieldModifierOffset && fieldModifier <= kMaxFieldModifierOffset );

	// Formulization
	{
		// Attacker attacks with weapon as baseline
		static_assert( kMaxAttackerBonus == 15, "Check balance" );
		static_assert( kMaxWeaponBonus == 20, "Check balance" );
		SimVal const rawAttack =
			0u +
			attackerBonus +
			weaponBonus * 2u;
		static constexpr SimVal kMaxUnscaledDamage = kMaxAttackerBonus + kMaxWeaponBonus * 2;
		static_assert( kMaxUnscaledDamage == 55, "Check balance" );
		RF_ASSERT( rawAttack <= kMaxUnscaledDamage );

		static_assert( kMaxFieldModifierOffset == 10, "Check balance" );
		SimDelta const totalModifier = fieldModifier;
		SimDelta modifierBonus = 0;
		if( totalModifier > 0 )
		{
			// Upscale damage
			modifierBonus = ( rawAttack * totalModifier ) / 20;
			RF_ASSERT( modifierBonus >= 0 );
			RF_ASSERT( modifierBonus <= rawAttack );
		}
		else if( totalModifier < 0 )
		{
			// Downscale damage
			modifierBonus = -1 + ( rawAttack * totalModifier ) / 20;
			RF_ASSERT( modifierBonus <= 0 );
			RF_ASSERT( modifierBonus >= -rawAttack );
		}
		else
		{
			// No bonus
			RF_ASSERT( totalModifier == 0 );
			modifierBonus = 0;
		}
		SimVal const scaledAttack = math::integer_cast<SimVal>( math::integer_cast<int16_t>( rawAttack ) + modifierBonus );
		RF_ASSERT( scaledAttack >= 0 );

		// No matter how bad your situation, the raw damage from the weapon
		//  itself should be able to make it through
		static constexpr SimVal kMaxScaledDamage = kMaxUnscaledDamage * 2;
		static_assert( kMaxScaledDamage == 110, "Check balance" );
		RF_ASSERT( scaledAttack <= kMaxScaledDamage );
		SimVal const applicableAttack = math::Clamp<SimVal>( weaponBonus, scaledAttack, kMaxScaledDamage );
		RF_ASSERT( applicableAttack > 0 );

		// Final value transform
		static constexpr SimVal kMaxFinalAttack = ( kMaxScaledDamage * 2u ) / 3u;
		static_assert( kMaxFinalAttack == 73, "Check balance" );
		SimVal const finalAttack = ( applicableAttack * 2u ) / 3u;
		RF_ASSERT( finalAttack <= kMaxFinalAttack );
		return math::Clamp<SimVal>( 1, finalAttack, kMaxFinalAttack );
	}
}



SimVal CombatEngine::LoCalcCounterFromAttackSwing( SimVal attackerComboMeter ) const
{
	return attackerComboMeter;
}



SimVal CombatEngine::LoCalcCounterFromAttackDamage( SimVal attackDamage ) const
{
	return attackDamage / 2u;
}



SimVal CombatEngine::LoCalcCounterFromElementDamage( SimVal elementDamage ) const
{
	return elementDamage;
}



SimVal CombatEngine::LoCalcElementDamage(
	SimVal attackerElemAtkStat,
	SimVal defenderElemDefStat,
	SimVal elementStrength,
	SimVal castedLevel,
	bool multiTarget,
	SimColor elementVsCaster,
	SimColor elementVsTarget,
	FieldColors const& elementField ) const
{
	RF_ASSERT( elementStrength > 0 );
	RF_ASSERT( elementStrength <= kMaxElementStrength );
	elementStrength = math::Clamp<SimVal>( 1u, elementStrength, kMaxElementStrength );

	RF_ASSERT( castedLevel >= element::kMinElementLevel );
	RF_ASSERT( castedLevel <= element::kMaxElementLevel );
	elementStrength = math::Clamp<SimVal>( 0u, elementStrength, kMaxElementStrength );

	// Penetration portion
	BreakClass const breakClass = LoCalcElementBreakClass( attackerElemAtkStat, defenderElemDefStat );
	SimVal const casterBonus = LoCalcCasterBonus( breakClass, attackerElemAtkStat, defenderElemDefStat );
	SimVal const elementBonus = LoCalcElementBonus( breakClass, elementStrength, castedLevel, multiTarget );

	// Elemental portion
	SimDelta const fieldModifier = LoCalcElementFieldModifier( elementVsTarget, elementField );
	SimDelta const affinityModifier = LoCalcCasterAffinityModifier( elementVsCaster );

	RF_ASSERT( casterBonus <= kMaxCasterBonus );
	RF_ASSERT( elementBonus <= kMaxElementBonus );
	RF_ASSERT( fieldModifier >= -kMaxFieldModifierOffset && fieldModifier <= kMaxFieldModifierOffset );
	RF_ASSERT( affinityModifier >= -kMaxAffinityModifierOffset && affinityModifier <= kMaxAffinityModifierOffset );

	// Formulization
	{
		// Caster casts with element as baseline
		static_assert( kMaxCasterBonus == 15, "Check balance" );
		static_assert( kMaxElementBonus == 80, "Check balance" );
		SimVal const rawCast =
			0u +
			casterBonus +
			elementBonus;
		static constexpr SimVal kMaxUnscaledDamage = kMaxCasterBonus + kMaxElementBonus;
		static_assert( kMaxUnscaledDamage == 95, "Check balance" );
		RF_ASSERT( rawCast <= kMaxUnscaledDamage );

		static_assert( kMaxFieldModifierOffset == 10, "Check balance" );
		static_assert( kMaxAffinityModifierOffset == 2, "Check balance" );
		SimDelta const totalModifier = fieldModifier + affinityModifier;
		SimDelta modifierBonus = 0;
		if( totalModifier > 0 )
		{
			// Upscale damage
			modifierBonus = ( rawCast * totalModifier ) / 20;
			RF_ASSERT( modifierBonus >= 0 );
			RF_ASSERT( modifierBonus <= rawCast );
		}
		else if( totalModifier < 0 )
		{
			// Downscale damage
			modifierBonus = -1 + ( rawCast * totalModifier ) / 20;
			RF_ASSERT( modifierBonus <= 0 );
			RF_ASSERT( modifierBonus >= -rawCast );
		}
		else
		{
			// No bonus
			RF_ASSERT( totalModifier == 0 );
			modifierBonus = 0;
		}
		SimVal const scaledAttack = math::integer_cast<SimVal>( math::integer_cast<int16_t>( rawCast ) + modifierBonus );
		RF_ASSERT( scaledAttack >= 0 );

		// No matter how bad your situation, the raw damage from the element
		//  itself should be able to make it through
		static constexpr SimVal kMaxScaledDamage = kMaxUnscaledDamage * 2;
		static_assert( kMaxScaledDamage == 190, "Check balance" );
		RF_ASSERT( scaledAttack <= kMaxScaledDamage );
		SimVal const applicableAttack = math::Clamp<SimVal>( elementBonus, scaledAttack, kMaxScaledDamage );
		RF_ASSERT( applicableAttack > 0 );

		// Final value transform
		static constexpr LargeSimVal kMaxFinalAttack = ( LargeSimVal( kMaxScaledDamage ) * 2u ) / 3u;
		static_assert( kMaxFinalAttack == 126, "Check balance" );
		LargeSimVal const finalAttack = ( math::integer_cast<LargeSimVal>( applicableAttack ) * 2u ) / 3u;
		RF_ASSERT( finalAttack <= kMaxFinalAttack );
		return math::integer_cast<SimVal>( math::Clamp<LargeSimVal>( 1, finalAttack, kMaxFinalAttack ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

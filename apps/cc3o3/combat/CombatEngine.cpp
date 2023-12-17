#include "stdafx.h"
#include "CombatEngine.h"

#include "cc3o3/combat/Attack.h"
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
	static constexpr SimVal kAttackBreakPoint = 5;
	if( attackerPhysAtkStat > defenderPhysDefStat + kAttackBreakPoint * 2 )
	{
		// Unreasonably stronger (>2xbreak)
		return BreakClass::Overwhelming;
	}
	else if( attackerPhysAtkStat > defenderPhysDefStat + kAttackBreakPoint )
	{
		// Crazy stronger (>break)
		return BreakClass::Critical;
	}
	else if( attackerPhysAtkStat >= defenderPhysDefStat )
	{
		// Equal or stronger (>)
		return BreakClass::Normal;
	}
	else if( attackerPhysAtkStat + kAttackBreakPoint >= defenderPhysDefStat )
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



SimVal CombatEngine::LoCalcWeaponBonus( BreakClass breakClass, SimVal attackStrength ) const
{
	static_assert( kMaxWeaponBonus == 20, "Check balance" );
	static constexpr SimVal kCriticalWeaponModifier = 4;
	static constexpr SimVal kNormalWeaponModifier = 3;
	static constexpr SimVal kWeakWeaponModifier = 2;
	static constexpr SimVal kIneffectiveWeaponModifier = 1;

	SimVal weaponBonus = 0;
	switch( breakClass )
	{
		case BreakClass::Overwhelming:
			// Same as normal, rely on attacker to push limits
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



SimVal CombatEngine::LoCalcAttackStaminaCost( SimVal attackStrength ) const
{
	RF_ASSERT( attackStrength > 0 );
	return attackStrength;
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
	static constexpr SimVal kMaxAttackStrength = 5;
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
		SimDelta fieldBonus = 0;
		if( fieldModifier > 0 )
		{
			// Upscale damage
			fieldBonus = ( rawAttack * fieldModifier ) / 20;
			RF_ASSERT( fieldBonus >= 0 );
			RF_ASSERT( fieldBonus <= rawAttack );
		}
		else if( fieldModifier < 0 )
		{
			// Downscale damage
			fieldBonus = -1 + ( rawAttack * fieldModifier ) / 20;
			RF_ASSERT( fieldBonus <= 0 );
			RF_ASSERT( fieldBonus >= -rawAttack );
		}
		else
		{
			// No bonus
			RF_ASSERT( fieldModifier == 0 );
			fieldBonus = 0;
		}
		SimVal const scaledAttack = math::integer_cast<SimVal>( math::integer_cast<int16_t>( rawAttack ) + fieldBonus );
		RF_ASSERT( scaledAttack >= 0 );

		// No matter how bad your situation, the raw damage from the weapon itself
		//  should be able to make it through
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

///////////////////////////////////////////////////////////////////////////////
}

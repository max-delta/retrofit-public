#include "stdafx.h"
#include "CombatEngine.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"


namespace RF { namespace cc { namespace combat {
///////////////////////////////////////////////////////////////////////////////

CombatEngine::CombatEngine( WeakPtr<file::VFS const> const& vfs )
	: mVfs( vfs )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

CombatEngine::SimVal CombatEngine::LoCalcAttackAccuracy( SimVal attackStrength ) const
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



CombatEngine::SimVal CombatEngine::LoCalcNewComboMeter( SimVal attackAccuracy, SimVal attackerTechniqStat ) const
{
	return 0u + attackAccuracy + attackerTechniqStat;
}



CombatEngine::SimVal CombatEngine::LoCalcContinueComboMeter( SimVal attackerComboMeter, SimVal attackAccuracy, SimVal attackerTechniqStat ) const
{
	return 0u + attackerComboMeter + attackAccuracy + attackerTechniqStat;
}



bool CombatEngine::LoCalcWillAttackHit( SimVal attackerComboMeter, SimVal defenderBalanceStat ) const
{
	return attackerComboMeter > defenderBalanceStat;
}



CombatEngine::SimVal CombatEngine::LoCalcAttackDamage( SimVal attackerPhysAtkStat, SimVal defenderPhysDefStat, SimVal attackStrength, SimColor color, SimDelta attackerFieldModifier, SimDelta defenderFieldModifier ) const
{
	static constexpr SimVal kMaxAttackStrength = 5;
	RF_ASSERT( attackStrength < kMaxAttackStrength );
	attackStrength = math::Clamp<SimVal>( 0u, attackStrength, kMaxAttackStrength );

	// Physical portion
	static constexpr SimVal kMaxAttackerBonus = 20;
	static constexpr SimVal kMaxWeaponBonus = 20;
	SimVal attackerBonus = 0;
	SimVal weaponBonus = 0;
	if( attackerPhysAtkStat >= defenderPhysDefStat + 10u )
	{
		// Crazy stronger
		attackerBonus = 20u;
		weaponBonus = 0u + attackStrength * 4;
	}
	else if( attackerPhysAtkStat >= defenderPhysDefStat )
	{
		// Equal or stronger
		attackerBonus = 10u + ( attackerPhysAtkStat - defenderPhysDefStat );
		weaponBonus = 0u + attackStrength * 2;
	}
	else if( attackerPhysAtkStat + 10u >= defenderPhysDefStat )
	{
		// Weaker, but not terribly so
		attackerBonus = ( attackerPhysAtkStat + 10u ) - defenderPhysDefStat;
		weaponBonus = 0u + attackStrength * 2;
	}
	else
	{
		// Heavily outclassed
		attackerBonus = 0;
		weaponBonus = attackStrength;
	}


	RF_ASSERT( attackerFieldModifier >= -kMaxFieldModiferOffset && attackerFieldModifier <= kMaxFieldModiferOffset );
	attackerFieldModifier = math::Clamp<SimDelta>( -kMaxFieldModiferOffset, attackerFieldModifier, kMaxFieldModiferOffset );
	RF_ASSERT( defenderFieldModifier >= -kMaxFieldModiferOffset && defenderFieldModifier <= kMaxFieldModiferOffset );
	defenderFieldModifier = math::Clamp<SimDelta>( -kMaxFieldModiferOffset, defenderFieldModifier, kMaxFieldModiferOffset );

	// Elemental portion
	static constexpr SimVal kMaxColorModifierOffset = kMaxFieldModiferOffset + 1;
	SimDelta colorModifier = 0;
	if( color == SimColor::Unrelated )
	{
		// Defender has no protection
		colorModifier = attackerFieldModifier;
	}
	else if( color == SimColor::Same )
	{
		// Defender can resist
		colorModifier = attackerFieldModifier - ( defenderFieldModifier + 1 );
	}
	else
	{
		RF_ASSERT( color == SimColor::Clash );
		colorModifier = 0;
	}


	RF_ASSERT( attackerBonus < kMaxAttackerBonus );
	attackerBonus = math::Clamp<SimVal>( 0u, attackerBonus, kMaxAttackerBonus );
	RF_ASSERT( weaponBonus < kMaxWeaponBonus );
	weaponBonus = math::Clamp<SimVal>( 0u, weaponBonus, kMaxWeaponBonus );
	RF_ASSERT( colorModifier >= -kMaxColorModifierOffset && colorModifier <= kMaxColorModifierOffset );
	colorModifier = math::Clamp<SimDelta>( -kMaxColorModifierOffset, colorModifier, kMaxColorModifierOffset );

	// Formulization
	static_assert( kMaxAttackerBonus == 20, "Check balance" );
	static_assert( kMaxWeaponBonus == 20, "Check balance" );
	static_assert( kMaxColorModifierOffset == 16, "Check balance" );
	int64_t const rawAttack =
		weaponBonus +
		attackerBonus +
		math::Clamp<SimDelta>( -4, colorModifier, 4 );

	// No matter how bad your situation, the raw damage from the weapon itself
	//  should be able to make it through
	static constexpr int64_t kMaxUnscaledDamage = 50;
	RF_ASSERT( rawAttack <= kMaxUnscaledDamage );
	int64_t const applicableAttack = math::Clamp<int64_t>( weaponBonus, rawAttack, kMaxUnscaledDamage );

	// Clashes scale up the final damage
	int64_t clashBonus = 0;
	if( color == SimColor::Clash )
	{
		clashBonus = applicableAttack / 2;
	}

	static constexpr int64_t kMaxFinalDamage = 75;
	int64_t const finalAttack = applicableAttack + clashBonus;
	RF_ASSERT( finalAttack <= kMaxFinalDamage );
	return math::integer_cast<SimVal>( math::Clamp<int64_t>( weaponBonus, finalAttack, kMaxFinalDamage ) );
}



CombatEngine::SimDelta CombatEngine::LoCalcFieldModifier( FieldBonuses const& fieldBonuses ) const
{
	SimDelta retVal = 0;
	for( SimDelta const& fieldBonus : fieldBonuses )
	{
		RF_ASSERT( fieldBonus >= -kMaxFieldBonusOffset && fieldBonus <= kMaxFieldBonusOffset );
		retVal += math::Clamp<SimDelta>( -kMaxFieldBonusOffset, fieldBonus, kMaxFieldBonusOffset );
	}
	RF_ASSERT( retVal >= -kMaxFieldModiferOffset && retVal <= kMaxFieldModiferOffset );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}}

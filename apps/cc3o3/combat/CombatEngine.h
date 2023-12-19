#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/array"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

class CombatEngine
{
	RF_NO_COPY( CombatEngine );

	//
	// Types and constants
public:
	using FieldColors = rftl::array<SimColor, kFieldSize>;

private:
	// Inputs to the system need to be capped
	static constexpr SimVal kMinAttackStrength = 1;
	static constexpr SimVal kMaxAttackStrength = 5;
	static constexpr SimVal kMinElementStrength = element::kMinElementLevel;
	static constexpr SimVal kMaxElementStrength = element::kMaxElementLevel;

	// Stat differences can only be so overpowered before they get capped
	static constexpr SimVal kMaxBreakDelta = 10;
	static constexpr SimVal kHalfMaxBreakDelta = 5;

	// Low-level attack calculations have limits
	static constexpr SimVal kMaxAttackerBonus = 15;
	static constexpr SimVal kMaxWeaponBonus = 20;

	// Low-level element calculations have limits
	static constexpr SimVal kMaxCasterBonus = 15;
	static constexpr SimVal kMaxElementBonus = 80;

	static constexpr SimVal kMaxFieldModifierOffset = 10;
	static constexpr SimVal kMaxAffinityModifierOffset = 2;


	//
	// Public methods
public:
	CombatEngine( WeakPtr<file::VFS const> const& vfs );


	//
	// Public methods (lookup evalutaions)
public:
	SimColor EvalInnates( element::InnateIdentifier lhs, element::InnateIdentifier rhs ) const;
	element::InnateIdentifier GenerateRandomInnate( uint32_t& lastGeneratorValue ) const;


	//
	// Public methods (display calculations)
public:
	DisplayVal DisplayHealth( LargeSimVal healthVal, EntityClass entityClass ) const;
	SignedDisplayVal DisplayStamina( SimDelta staminaVal, EntityClass entityClass ) const;
	DisplayVal DisplayCharge( SimVal chargeVal, EntityClass entityClass ) const;
	DisplayVal DisplayStandardStat( SimVal statVal, EntityClass entityClass ) const;
	char DisplayInnateGlyphAscii( element::InnateIdentifier innate ) const;


	//
	// Public methods (high-level calculations)
public:
	// Physical attack
	AttackResult HiCalcAttack( AttackProfile const& profile ) const;

	// Elemental cast
	CastDamageResult HiCalcCast( CastDamageProfile const& profile ) const;


	//
	// Public methods (low-level calculations)
public:
	// Max health drives the balance and tuning of all combat
	LargeSimVal LoCalcMaxHealth( SimVal healthStat, EntityClass entityClass ) const;

	// Stamina regains over time
	SimVal LoCalcIdleStaminaGainAlliedTurn() const;
	SimVal LoCalcIdleStaminaGainOpposingTurn() const;

	// Attacks and elements have thresholds where calculations start changing
	BreakClass LoCalcAttackBreakClass( SimVal attackerPhysAtkStat, SimVal defenderPhysDefStat ) const;
	BreakClass LoCalcElementBreakClass( SimVal attackerElemAtkStat, SimVal defenderElemDefStat ) const;
	SimVal LoCalcAttackerBonus( BreakClass breakClass, SimVal attackerPhysAtkStat, SimVal defenderPhysDefStat ) const;
	SimVal LoCalcCasterBonus( BreakClass breakClass, SimVal attackerElemAtkStat, SimVal defenderElemDefStat ) const;
	SimVal LoCalcWeaponBonus( BreakClass breakClass, SimVal attackStrength ) const;
	SimVal LoCalcElementBonus( BreakClass breakClass, SimVal elementStrength, SimVal castedLevel, bool multiTarget ) const;

	// Attacks and elements are affected by colors
	SimDelta LoCalcAttackFieldModifier( SimColor attackVsTarget, FieldColors const& attackerField ) const;
	SimDelta LoCalcElementFieldModifier( SimColor elementVsTarget, FieldColors const& elementField ) const;
	SimDelta LoCalcCasterAffinityModifier( SimColor elementVsCaster ) const;

	// Attacks cost stamina based on strength
	SimVal LoCalcAttackStaminaCost( SimVal attackStrength ) const;

	// Elements cost stamina
	SimVal LoCalcElementStaminaCost() const;

	// Attacks grant charge based on strength
	SimVal LoCalcAttackChargeGain( SimVal attackStrength ) const;

	// Casts cost charge based on level
	SimVal LoCalcElementChargeCost( SimVal castedLevel ) const;

	// Attack accuracy is affected by the strength of the attack
	SimVal LoCalcAttackAccuracy( SimVal attackStrength ) const;

	// Attacker builds combo based on attacking with techniq
	SimVal LoCalcNewComboMeter(
		SimVal attackAccuracy,
		SimVal attackerTechniqStat,
		SimVal defenderBalanceStat ) const;
	SimVal LoCalcContinueComboMeter(
		SimVal attackerComboMeter,
		SimVal attackAccuracy,
		SimVal attackerTechniqStat,
		SimVal defenderBalanceStat ) const;

	// Attacker combo must overcome defender balance
	SimVal LoCalcMinComboToHit( SimVal defenderBalanceStat ) const;
	bool LoCalcWillAttackHit( SimVal attackerComboMeter, SimVal defenderBalanceStat ) const;

	// Attacks are physical against defense, but color also affects them
	SimVal LoCalcAttackDamage(
		SimVal attackerPhysAtkStat,
		SimVal defenderPhysDefStat,
		SimVal attackStrength,
		SimColor attackVsTarget,
		FieldColors const& attackerField ) const;

	// Swinging (hit or otherwise) increases the counter gauge
	SimVal LoCalcCounterFromAttackSwing( SimVal attackerComboMeter ) const;

	// Dealing attack damage increases the counter gauge
	SimVal LoCalcCounterFromAttackDamage( SimVal attackDamage ) const;

	// Dealing elemental damage increases the counter gauge
	SimVal LoCalcCounterFromElementDamage( SimVal elementDamage ) const;

	// Elements are elemental against defense, and color affects them
	SimVal LoCalcElementDamage(
		SimVal attackerElemAtkStat,
		SimVal defenderElemDefStat,
		SimVal elementStrength,
		SimVal castedLevel,
		bool multiTarget,
		SimColor elementVsCaster,
		SimColor elementVsTarget,
		FieldColors const& elementField ) const;


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
};

///////////////////////////////////////////////////////////////////////////////
}

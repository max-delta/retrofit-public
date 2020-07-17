#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/array"


namespace RF { namespace cc { namespace combat {
///////////////////////////////////////////////////////////////////////////////

class CombatEngine
{
	RF_NO_COPY( CombatEngine );

	//
	// Types and constants
public:
	using FieldColors = rftl::array<SimColor, kFieldSize>;


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
	DisplayVal DisplayStandardStat( SimVal statVal, EntityClass entityClass ) const;


	//
	// Public methods (high-level calculations)
public:
	// Physical attack
	AttackResult HiCalcAttack( AttackProfile const& profile ) const;


	//
	// Public methods (low-level calculations)
public:
	// Max health drives the balance and tuning of all combat
	LargeSimVal LoCalcMaxHealth( SimVal healthStat, EntityClass entityClass ) const;

	// Attack accuracy is affected by the strength of the attack
	SimVal LoCalcAttackAccuracy( SimVal attackStrength ) const;

	// Attacker builds combo based on attacking with techniq
	SimVal LoCalcNewComboMeter( SimVal attackAccuracy, SimVal attackerTechniqStat, SimVal defenderBalanceStat ) const;
	SimVal LoCalcContinueComboMeter( SimVal attackerComboMeter, SimVal attackAccuracy, SimVal attackerTechniqStat, SimVal defenderBalanceStat ) const;

	// Attacker combo must overcome defender balance
	SimVal LoCalcMinComboToHit( SimVal defenderBalanceStat ) const;
	bool LoCalcWillAttackHit( SimVal attackerComboMeter, SimVal defenderBalanceStat ) const;

	// Attacks are physical against defense, but color also affects them
	SimVal LoCalcAttackDamage( SimVal attackerPhysAtkStat, SimVal defenderPhysDefStat, SimVal attackStrength, SimColor attackVsTarget, FieldColors const& attackerField ) const;

	// Swinging (hit or otherwise) increases the counter gauge
	SimVal LoCalcCounterFromAttackSwing( SimVal attackerComboMeter ) const;

	// Dealing attack damage increases the counter gauge
	SimVal LoCalcCounterFromAttackDamage( SimVal attackDamage ) const;


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
};

///////////////////////////////////////////////////////////////////////////////
}}}

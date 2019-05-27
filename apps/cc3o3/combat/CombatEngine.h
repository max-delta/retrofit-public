#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/array"


namespace RF { namespace cc { namespace combat {
///////////////////////////////////////////////////////////////////////////////

class CombatEngine
{
	RF_NO_COPY( CombatEngine );

	//
	// Enums
public:
	enum class SimColor : uint8_t
	{
		Unrelated = 0,
		Same,
		Clash
	};


	//
	// Types and constants
public:
	using SimVal = uint8_t;
	using SimDelta = int8_t;
	static constexpr size_t kFieldSize = 5;
	using FieldColors = rftl::array<SimColor, kFieldSize>;


	//
	// Public methods
public:
	CombatEngine( WeakPtr<file::VFS const> const& vfs );


	//
	// Public methods (low-level calculations)
public:
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

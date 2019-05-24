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
	using FieldBonuses = rftl::array<SimDelta, kFieldSize>;
	static constexpr size_t kNumColors = 6;
	static constexpr SimVal kMaxFieldBonusOffset = kNumColors / 2;
	static constexpr SimVal kMaxFieldModiferOffset = kFieldSize * kMaxFieldBonusOffset;


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
	SimVal LoCalcNewComboMeter( SimVal attackAccuracy, SimVal attackerTechniqStat ) const;
	SimVal LoCalcContinueComboMeter( SimVal attackerComboMeter, SimVal attackAccuracy, SimVal attackerTechniqStat ) const;

	// Attacker combo must overcome defender balance
	bool LoCalcWillAttackHit( SimVal attackerComboMeter, SimVal defenderBalanceStat ) const;

	// Attacks are physical against defense, but color also affects them
	SimVal LoCalcAttackDamage( SimVal attackerPhysAtkStat, SimVal defenderPhysDefStat, SimVal attackStrength, SimColor color, SimDelta attackerFieldModifier, SimDelta defenderFieldModifier ) const;

	SimDelta LoCalcFieldModifier( FieldBonuses const& fieldBonuses ) const;


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
};

///////////////////////////////////////////////////////////////////////////////
}}}

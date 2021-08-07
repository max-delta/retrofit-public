#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

struct AttackProfile
{
	SimVal mAttackStrength = 0;

	// Try to hit
	bool mNewCombo = false;
	SimVal mComboMeter = 0;
	SimVal mAttackerTechnique = 0;
	SimVal mDefenderBalance = 0;

	// Deal the damage
	SimVal mAttackerPhysicalAttack = 0;
	SimVal mDefenderPhysicalDefense = 0;
	element::InnateIdentifier mAttackerInnate = {};
	element::InnateIdentifier mDefenderInnate = {};
	rftl::array<element::InnateIdentifier, kFieldSize> mInfluence = {};
};



struct AttackResult
{
	SimVal mNewComboMeter = 0;
	bool mHit = false;
	SimVal mDamage = 0;
	SimVal mCoungerGuageIncrease = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

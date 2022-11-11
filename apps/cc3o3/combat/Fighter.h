#pragma once
#include "project.h"

#include "cc3o3/combat/CombatantID.h"
#include "cc3o3/elements/ElementFwd.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

struct Fighter
{
	EntityClass mEntityClass = EntityClass::Invalid;

	element::InnateIdentifier mInnate = element::kInvalidInnateIdentifier;

	LargeSimVal mMaxHealth = 0;
	LargeSimVal mCurHealth = 0;

	SimDelta mMaxStamina = 0;
	SimDelta mCurStamina = 0;

	SimVal mMaxCharge = 0;
	SimVal mCurCharge = 0;

	SimVal mPhysAtk = 0; // Punch harder
	SimVal mPhysDef = 0; // Take less from punches

	SimVal mElemAtk = 0; // Cast harder
	SimVal mElemDef = 0; // Take less from casts

	SimVal mBalance = 0; // Higher initial shield
	SimVal mTechniq = 0; // Faster combo growth

	SimVal mComboMeter = 0;
	FighterID mComboTarget = {}; // Changing targets drops combo meter
};

///////////////////////////////////////////////////////////////////////////////
}

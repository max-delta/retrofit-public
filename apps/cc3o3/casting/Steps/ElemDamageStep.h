#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"
#include "cc3o3/combat/CombatFwd.h"

#include "GameAction/Step.h"


namespace RF::cc::cast::step {
///////////////////////////////////////////////////////////////////////////////

// Applies damage to a target, using elemental damage calculations
class ElemDamageStep : public act::Step
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	virtual UniquePtr<act::Context> Execute( act::Environment const& env, act::Context& ctx ) const override;


	//
	// Public data
public:
	// Damage calculation normally uses the strength from the base level and
	//  any upleveling or downleveling, but it can be further modified here
	combat::SimDelta mElemStrengthLevelModifier = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

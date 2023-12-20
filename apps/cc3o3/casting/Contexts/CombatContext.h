#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"
#include "cc3o3/combat/CombatInstance.h"

#include "GameAction/Context.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

class CombatContext : public act::Context
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_DEFAULT_COPY( CombatContext );

	//
	// Public methods
public:
	explicit CombatContext(
		combat::CombatInstance const& combatInstance );

	virtual UniquePtr<Context> Clone() const override;


	//
	// Public data
public:
	// The combat instance that is being operated on
	// NOTE: This is a copy, not a reference, so it's expected to be copied in
	//  and later copied back out of the context when all of the execution is
	//  fully completed
	combat::CombatInstance mCombatInstance;

	// Many actions are source->target between combatants
	// NOTE: May be the same in cases of self-targeting
	// NOTE: Can be explicitly invalid if not present
	combat::FighterID mSourceFighter = {};
	combat::FighterID mTargetFighter = {};

	// When casting an element, its color can affect how it interacts with the
	//  source and the target's color
	// NOTE: Can be invalid when not performing a cast, or some other special
	//  colorless action
	element::InnateIdentifier mElementColor = element::kInvalidInnateIdentifier;

	// When casting an element, the strength of the element is usually the
	//  same as the natural level of the element, which can be different than
	//  the level it was cast at
	// NOTE: Can be zero when not performing a cast
	combat::SimVal mElementStrength = 0;

	// When casting an element, the level of the cast can be different than the
	//  natural level or strength of the element
	// NOTE: Can be zero when not performing a cast
	element::ElementLevel mCastedLevel = 0;

	// Some actions are multi-target, which could affect things like damage
	//  calculations, to reduce their effects and prevent them from being used
	//  as an overcentralizing catch-all action
	bool mMultiTarget = false;
};

///////////////////////////////////////////////////////////////////////////////
}

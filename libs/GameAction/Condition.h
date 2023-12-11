#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Base class for checking conditions like 'is dead' or 'health >= X'
class GAMEACTION_API Condition
{
	//
	// Public methods
public:
	Condition();
	virtual ~Condition();

	// Evaluate a condition on a given context in a given environment
	// NOTE: Assumed to not change context, custom contexts need to provide
	//  one or more designated mutable portions if they wish to allow certain
	//  conditions to bypass this
	virtual bool Evaluate( Environment const& env, Context const& ctx ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"
#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

class CastingEngine
{
	RF_NO_COPY( CastingEngine );

	//
	// Public methods
public:
	CastingEngine(
		WeakPtr<combat::CombatEngine const> const& combatEngine,
		WeakPtr<element::ElementDatabase const> const& elementDatabase );


	//
	// Private data
private:
	WeakPtr<combat::CombatEngine const> mCombatEngine;
	WeakPtr<element::ElementDatabase const> mElementDatabase;
};

///////////////////////////////////////////////////////////////////////////////
}

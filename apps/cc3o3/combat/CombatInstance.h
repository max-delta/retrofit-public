#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

class CombatInstance
{
	RF_NO_COPY( CombatInstance );

	//
	// Public methods
public:
	CombatInstance( WeakPtr<CombatEngine const> const& combatEngine );


	//
	// Private data
private:
	WeakPtr<CombatEngine const> mCombatEngine;
};

///////////////////////////////////////////////////////////////////////////////
}

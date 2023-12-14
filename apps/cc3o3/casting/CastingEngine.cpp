#include "stdafx.h"
#include "CastingEngine.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

CastingEngine::CastingEngine(
	WeakPtr<combat::CombatEngine const> const& combatEngine,
	WeakPtr<element::ElementDatabase const> const& elementDatabase )
	: mCombatEngine( combatEngine )
	, mElementDatabase( elementDatabase )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}

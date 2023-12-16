#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

struct Field
{
	using Influence = rftl::array<element::InnateIdentifier, kFieldSize>;

	Influence mInfluence = {};
};

///////////////////////////////////////////////////////////////////////////////
}

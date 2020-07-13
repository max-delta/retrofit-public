#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

struct Field
{
	rftl::array<element::InnateIdentifier, kFieldSize> mInfluence = {};
};

///////////////////////////////////////////////////////////////////////////////
}

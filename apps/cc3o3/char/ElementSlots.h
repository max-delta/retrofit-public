#pragma once
#include "project.h"

#include "cc3o3/char/CharFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

struct ElementSlots
{
	using Slot = element::ElementIdentifier;
	using Grid = rftl::array<Slot, kMaxElementLevels * kMaxSlotsPerElementLevel>;

	Grid mGrid = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}

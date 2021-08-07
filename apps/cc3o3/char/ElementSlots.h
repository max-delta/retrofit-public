#pragma once
#include "project.h"

#include "cc3o3/char/CharFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

struct ElementSlots
{
	//
	// Types and constants
public:
	using Slot = element::ElementIdentifier;
	using Column = rftl::array<Slot, kMaxSlotsPerElementLevel>;
	using Grid = rftl::array<Column, kMaxElementLevels>;


	//
	// Public methods
public:
	ElementSlots() = default;

	Slot const& At( ElementSlotIndex const& location ) const;
	Slot& At( ElementSlotIndex const& location );


	//
	// Public data
public:
	Grid mGrid = {};
};

///////////////////////////////////////////////////////////////////////////////
}

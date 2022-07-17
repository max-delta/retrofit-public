#pragma once
#include "project.h"

#include "cc3o3/ui/ElementSlotDisplayCache.h"
#include "cc3o3/char/CharFwd.h"
#include "cc3o3/state/StateFwd.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

class ElementGridDisplayCache
{
	//
	// Types and constants
public:
	using Slot = ElementSlotDisplayCache;
	using Column = rftl::array<Slot, character::kMaxSlotsPerElementLevel>;

private:
	using Grid = rftl::array<Column, element::kNumElementLevels>;

	//
	// Public methods
public:
	ElementGridDisplayCache() = default;

	void UpdateFromCharacter( state::ObjectRef const& character, bool includeStrings );

	void DarkenAll();

	Slot const& GetSlotRef( character::ElementSlotIndex const& location ) const;
	Column const& GetColumnRef( element::ElementLevel const& level ) const;


	//
	// Private data
private:
	Grid mGrid;
};

///////////////////////////////////////////////////////////////////////////////
}

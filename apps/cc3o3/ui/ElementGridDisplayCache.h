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
	using Grid = rftl::array<Column, character::kMaxElementLevels>;

	//
	// Public methods
public:
	ElementGridDisplayCache() = default;

	void UpdateFromCharacter( state::ObjectRef const& character, bool includeStrings );

	void DarkenAll();

	Grid const& GetGridRef() const;


	//
	// Private data
private:
	Grid mGrid;
};

///////////////////////////////////////////////////////////////////////////////
}

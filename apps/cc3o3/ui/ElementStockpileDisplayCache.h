#pragma once
#include "project.h"

#include "cc3o3/ui/ElementSlotDisplayCache.h"
#include "cc3o3/char/CharFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "rftl/vector"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

class ElementStockpileDisplayCache
{
	//
	// Types and constants
public:
	using Slot = ElementSlotDisplayCache;
	using Stockpile = rftl::vector<Slot>;

	//
	// Public methods
public:
	ElementStockpileDisplayCache() = default;

	void UpdateFromCompany( state::ObjectRef const& company );
	Stockpile const& GetStockpileRef() const;


	//
	// Private data
private:
	Stockpile mStockpile;
};

///////////////////////////////////////////////////////////////////////////////
}

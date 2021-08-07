#pragma once
#include "project.h"

#include "cc3o3/ui/ElementTilesetValues.h"
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/string"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

class ElementSlotDisplayCache
{
	//
	// Public methods
public:
	void UpdateFromDesc( element::ElementDesc const& desc, bool includeStrings );
	void Unallocate();
	void Darken();

	//
	// Public data
public:
	element::ElementIdentifier mIdentifier = element::kInvalidElementIdentifier;
	rftl::string mName = {};
	ElementTilesetIndex mTilesetIndex = ElementTilesetIndex::Empty;
};

///////////////////////////////////////////////////////////////////////////////
}

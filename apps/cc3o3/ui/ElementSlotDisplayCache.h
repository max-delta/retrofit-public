#pragma once
#include "project.h"

#include "cc3o3/ui/ElementTilesetValues.h"

#include "rftl/string"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

class ElementSlotDisplayCache
{
	//
	// Public data
public:
	rftl::string mName = {};
	ElementTilesetIndex mTilesetIndex = ElementTilesetIndex::Empty;
};

///////////////////////////////////////////////////////////////////////////////
}

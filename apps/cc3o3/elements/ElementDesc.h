#pragma once
#include "cc3o3/elements/ElementFwd.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

struct ElementDesc
{
	ElementIdentifier mIdentifier = kInvalidElementIdentifier;
	InnateIdentifier mInnate = kInvalidInnateIdentifier;
	size_t mBaseLevel = 0;
	size_t mMinLevel = 0;
	size_t mMaxLevel = 0;

	// Only the identifier matters during comparison
	bool operator==( ElementDesc const& rhs ) const;
	bool operator==( ElementIdentifier const& rhs ) const;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "cc3o3/elements/ElementFwd.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

struct ElementDesc
{
	ElementIdentifier mIdentifier = kInvalidElementIdentifier;
	InnateIdentifier mInnate = kInvalidInnateIdentifier;
	ElementLevel mBaseLevel = kInvalidElementLevel;
	ElementLevel mMinLevel = kInvalidElementLevel;
	ElementLevel mMaxLevel = kInvalidElementLevel;
	size_t mStackSize = 0;
	bool mChain = false;

	// Only the identifier matters during comparison
	bool operator==( ElementDesc const& rhs ) const;
	bool operator==( ElementIdentifier const& rhs ) const;
};

///////////////////////////////////////////////////////////////////////////////

bool SortPredicateDescInnate( ElementDesc const& lhs, ElementDesc const& rhs );
bool SortPredicateDescLevel( ElementDesc const& lhs, ElementDesc const& rhs );

///////////////////////////////////////////////////////////////////////////////
}

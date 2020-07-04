#include "stdafx.h"
#include "ElementDesc.h"

#include "cc3o3/elements/IdentifierUtils.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

bool ElementDesc::operator==( ElementDesc const& rhs ) const
{
	return *this == rhs.mIdentifier;
}



bool ElementDesc::operator==( ElementIdentifier const& rhs ) const
{
	return mIdentifier == rhs;
}

///////////////////////////////////////////////////////////////////////////////

bool SortPredicateDescInnate( ElementDesc const& lhs, ElementDesc const& rhs )
{
	return SortPredicateInnate( lhs.mInnate, rhs.mInnate );
}



bool SortPredicateDescLevel( ElementDesc const& lhs, ElementDesc const& rhs )
{
	if( lhs.mBaseLevel != rhs.mBaseLevel )
	{
		return lhs.mBaseLevel < rhs.mBaseLevel;
	}

	if( lhs.mMinLevel != rhs.mMinLevel )
	{
		return lhs.mMinLevel < rhs.mMinLevel;
	}

	return lhs.mMaxLevel < rhs.mMaxLevel;
}

///////////////////////////////////////////////////////////////////////////////
}

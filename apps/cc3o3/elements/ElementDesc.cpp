#include "stdafx.h"
#include "ElementDesc.h"


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
}

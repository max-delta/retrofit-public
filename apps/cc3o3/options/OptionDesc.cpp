#include "stdafx.h"
#include "OptionDesc.h"

#include "rftl/limits"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

bool OptionDesc::List::FindIdentifier( size_t& index, rftl::string_view identifier ) const
{
	size_t const numItems = mItems.size();
	for( size_t i = 0; i < numItems; i++ )
	{
		Item const& item = mItems.at( i );
		if( item.mIdentifier == identifier )
		{
			index = i;
			return true;
		}
	}

	index = rftl::numeric_limits<size_t>::max();
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}

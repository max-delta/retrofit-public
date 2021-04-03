#pragma once
#include "rftl/extension/algorithms.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename DestContainer, typename SourceContainer>
auto string_copy( DestContainer& dest, SourceContainer const& source ) -> typename DestContainer::iterator
{
	if( dest.begin() == dest.end() )
	{
		return dest.end();
	}

	typename DestContainer::iterator const afterLastElem = copy_overwrite_clamped( dest, source );

	if( afterLastElem == dest.end() )
	{
		*dest.rbegin() = {};
		return dest.end();
	}
	else
	{
		*afterLastElem = {};
		return afterLastElem + 1;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

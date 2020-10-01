#pragma once
#include "algorithms.h"

#include "rftl/algorithm"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Container, typename UnaryPredicate>
inline auto erase_if( Container& container, UnaryPredicate const& condition ) -> typename Container::iterator
{
	return container.erase(
		rftl::remove_if( container.begin(), container.end(), condition ),
		container.end() );
}

///////////////////////////////////////////////////////////////////////////////
}

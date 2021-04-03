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



template<typename Container>
inline size_t erase_duplicates( Container& container )
{
	size_t const originalSize = container.size();
	typename Container::const_iterator const newEnd = rftl::unique( container.begin(), container.end() );
	container.erase( newEnd, container.end() );
	return container.size() - originalSize;
}



template<typename DestContainer, typename SourceContainer>
auto copy_overwrite_clamped( DestContainer& dest, SourceContainer const& source ) -> typename DestContainer::iterator
{
	typename DestContainer::iterator destIter = dest.begin();
	typename SourceContainer::const_iterator sourceIter = source.begin();
	while( destIter != dest.end() && sourceIter != source.end() )
	{
		*destIter = *sourceIter;
		destIter++;
		sourceIter++;
	}

	return destIter;
}

///////////////////////////////////////////////////////////////////////////////
}

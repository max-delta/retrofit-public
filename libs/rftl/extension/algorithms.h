#pragma once


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Container, typename UnaryPredicate>
auto erase_if( Container& container, UnaryPredicate const& condition ) -> typename Container::iterator;

template<typename Container>
size_t erase_duplicates( Container& container );

template<typename DestContainer, typename SourceContainer>
auto copy_overwrite_clamped( DestContainer& dest, SourceContainer const& source ) -> typename DestContainer::iterator;

///////////////////////////////////////////////////////////////////////////////
}

#include "algorithms.inl"

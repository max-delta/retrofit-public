#pragma once


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Container, typename UnaryPredicate>
auto erase_if( Container& container, UnaryPredicate const& condition ) -> typename Container::iterator;

template<typename Container>
size_t erase_duplicates( Container& container );

///////////////////////////////////////////////////////////////////////////////
}

#include "algorithms.inl"

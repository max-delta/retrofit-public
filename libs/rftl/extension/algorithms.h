#pragma once


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Container, typename UnaryPredicate>
auto erase_if( Container& container, UnaryPredicate const& condition ) -> typename Container::iterator;

///////////////////////////////////////////////////////////////////////////////
}

#include "algorithms.inl"

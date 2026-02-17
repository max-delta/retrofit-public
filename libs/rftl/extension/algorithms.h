#pragma once


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Container>
bool contains( Container const& container, typename Container::value_type const& value );
template<typename T, size_t N>
bool contains( T const ( &container )[N], T const& value );

template<typename Container>
void pop_front_via_erase( Container& container );

template<typename Container>
auto erase_all( Container& container, typename Container::value_type const& value ) -> typename Container::iterator;

template<typename Container, typename UnaryPredicate>
auto erase_if( Container& container, UnaryPredicate const& condition ) -> typename Container::iterator;

template<typename Container>
size_t erase_duplicates( Container& container );

template<typename Container>
auto replace( Container& container, typename Container::value_type const& oldVal, typename Container::value_type const& newVal ) -> Container&;

template<typename Container>
auto replace_copy( Container const& container, typename Container::value_type const& oldVal, typename Container::value_type const& newVal ) -> Container;

template<typename Container>
auto concatenate( Container const& container, typename Container::value_type const& value ) -> Container;

template<typename DestContainer, typename SourceContainer>
constexpr auto copy_overwrite_clamped( DestContainer& dest, SourceContainer const& source ) -> typename DestContainer::iterator;

template<class Iter, class Unary>
constexpr auto partition_point_iter( Iter first, Iter last, Unary predicate ) -> Iter;

///////////////////////////////////////////////////////////////////////////////
}

#include "algorithms.inl"

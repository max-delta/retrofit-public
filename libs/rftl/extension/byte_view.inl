#pragma once
#include "byte_view.h"

#include "core/macros.h"

#include "rftl/cstdlib"
#include "rftl/cstdint"
#include "rftl/cstring"
#include "rftl/limits"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

inline byte_view::byte_view( void* data, size_type size )
	: mData( data )
	, mSize( size )
{
	//
}



inline byte_view::byte_view( void const* data, size_type size )
	: mData( data )
	, mSize( size )
{
	//
}



inline byte_view::byte_view( nullptr_t, size_type size )
	: byte_view()
{
	//
}



inline byte_view::byte_view( const_iterator first, const_iterator term )
	: mData( first )
{
	RF_ASSERT( first <= term );
	mSize = reinterpret_cast<uintptr_t>( term ) - reinterpret_cast<uintptr_t>( first );
}



template<class InputIterator>
inline byte_view::byte_view( InputIterator first, InputIterator term )
	: mData( static_cast<const_iterator>( &*first ) )
{
	RF_ASSERT( first <= term );
	auto const distance = rftl::distance( first, term );
	RF_ASSERT( distance >= 0 );
	mSize = static_cast<size_type>( distance ) * sizeof( typename rftl::iterator_traits<InputIterator>::value_type );
}
template<>
inline byte_view::byte_view( void* first, void* term )
	: byte_view( static_cast<const_iterator>( first ), static_cast<const_iterator>( term ) ) { }
template<>
inline byte_view::byte_view( nullptr_t first, nullptr_t term )
	: byte_view() { }



template<class Type>
inline byte_view::byte_view( Type const* data )
	: byte_view( data, sizeof( Type ) )
{
	//
}



template<typename Type>
inline Type const& byte_view::at( size_type pos ) const
{
	if( pos + sizeof( Type ) > size() )
	{
		rftl::abort();
	}
	return this->operator[]<Type>( pos );
}



template<typename Type>
inline Type const& byte_view::operator[]( size_type pos ) const
{
	RF_ASSERT_MSG( pos + sizeof( Type ) <= size(), "Invalid pos accesses past end" );
	uint8_t const* const bytes = reinterpret_cast<uint8_t const*>( data() ) + pos;
	return *reinterpret_cast<Type const*>( bytes );
}



template<typename Type>
inline Type const& byte_view::front() const
{
	return at( 0 );
}



template<typename Type>
inline Type const& byte_view::back() const
{
	return at( size() - sizeof( Type ) );
}



inline byte_view::value_type const* byte_view::data() const
{
	return mData;
}



inline byte_view::iterator byte_view::begin()
{
	return cbegin();
}



inline byte_view::const_iterator byte_view::begin() const
{
	return cbegin();
}



inline byte_view::const_iterator byte_view::cbegin() const
{
	return data();
}



inline byte_view::iterator byte_view::end()
{
	return cend();
}



inline byte_view::const_iterator byte_view::end() const
{
	return cend();
}



inline byte_view::const_iterator byte_view::cend() const
{
	return reinterpret_cast<uint8_t const*>( data() ) + size();
}



inline byte_view::reverse_iterator byte_view::rbegin()
{
	return crbegin();
}



inline byte_view::const_reverse_iterator byte_view::rbegin() const
{
	return crbegin();
}



inline byte_view::const_reverse_iterator byte_view::crbegin() const
{
	return const_reverse_iterator( reinterpret_cast<void_alias const*>( cend() ) );
}



inline byte_view::reverse_iterator byte_view::rend()
{
	return crend();
}



inline byte_view::const_reverse_iterator byte_view::rend() const
{
	return crend();
}



inline byte_view::const_reverse_iterator byte_view::crend() const
{
	return const_reverse_iterator( reinterpret_cast<void_alias const*>( cbegin() ) );
}



inline bool byte_view::empty() const
{
	return size() == 0;
}



inline byte_view::size_type byte_view::size() const
{
	return mSize;
}



inline byte_view::size_type byte_view::length() const
{
	return size();
}



inline constexpr byte_view::size_type byte_view::max_size() const
{
	return rftl::numeric_limits<size_type>::max();
}



inline void byte_view::remove_prefix( size_type size )
{
	RF_ASSERT( size <= this->size() );
	mSize -= size;
	mData = reinterpret_cast<uint8_t const*>( mData ) + size;
}



inline void byte_view::remove_suffix( size_type size )
{
	RF_ASSERT( size <= this->size() );
	mSize -= size;
}



inline byte_view byte_view::substr( size_type pos )
{
	RF_ASSERT( pos <= size() );
	return byte_view( reinterpret_cast<uint8_t const*>( begin() ) + pos, end() );
}



inline byte_view byte_view::substr( size_type pos, size_type size )
{
	RF_ASSERT( pos <= this->size() );
	RF_ASSERT( pos + size <= this->size() );
	return byte_view(
		reinterpret_cast<uint8_t const*>( begin() ) + pos,
		reinterpret_cast<uint8_t const*>( begin() ) + size );
}

///////////////////////////////////////////////////////////////////////////////

inline bool operator==( byte_view const& lhs, byte_view const& rhs )
{
	return lhs.size() == rhs.size() && rftl::memcmp( lhs.data(), rhs.data(), lhs.size() ) == 0;
}



inline bool operator!=( byte_view const& lhs, byte_view const& rhs )
{
	return lhs.size() != rhs.size() || rftl::memcmp( lhs.data(), rhs.data(), lhs.size() ) != 0;
}

///////////////////////////////////////////////////////////////////////////////
}

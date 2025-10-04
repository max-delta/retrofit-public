#pragma once
#include "byte_span.h"

#include "core/macros.h"

#include "rftl/cstdlib"
#include "rftl/cstdint"
#include "rftl/cstring"
#include "rftl/limits"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

inline byte_span::byte_span( void* data, size_type size )
	: mData( data )
	, mSize( size )
{
	//
}



inline byte_span::byte_span( nullptr_t, size_type size )
	: byte_span()
{
	//
}



inline byte_span::byte_span( iterator first, iterator term )
	: mData( first )
{
	RF_ASSERT( first <= term );
	mSize = reinterpret_cast<uintptr_t>( term ) - reinterpret_cast<uintptr_t>( first );
}



template<class InputIterator>
inline byte_span::byte_span( InputIterator first, InputIterator term )
	: mData( static_cast<iterator>( &*first ) )
{
	RF_ASSERT( first <= term );
	auto const distance = rftl::distance( first, term );
	RF_ASSERT( distance >= 0 );
	mSize = static_cast<size_type>( distance ) * sizeof( typename rftl::iterator_traits<InputIterator>::value_type );
}
template<>
inline byte_span::byte_span( void* first, void* term )
	: byte_span( static_cast<iterator>( first ), static_cast<iterator>( term ) ) {}
template<>
inline byte_span::byte_span( nullptr_t first, nullptr_t term )
	: byte_span() {}



template<class Type>
inline byte_span::byte_span( Type* data )
	: byte_span( data, sizeof( Type ) )
{
	//
}



template<typename T>
inline span<T, dynamic_extent> byte_span::to_typed_span() const
{
	static constexpr size_t ElemSize = sizeof( T );
	size_t const numElements = mSize / ElemSize;
	RF_ASSERT_MSG( mSize % ElemSize == 0, "Conversion to typed span is losing data" );
	return span<T, dynamic_extent>( reinterpret_cast<T*>( mData ), numElements );
}



template<typename Type>
inline Type& byte_span::at( size_type pos ) const
{
	if( pos + sizeof( Type ) > size() )
	{
		rftl::abort();
	}
	return this->operator[]<Type>( pos );
}



template<typename Type>
inline Type& byte_span::operator[]( size_type pos ) const
{
	RF_ASSERT_MSG( pos + sizeof( Type ) <= size(), "Invalid pos accesses past end" );
	uint8_t* const bytes = reinterpret_cast<uint8_t*>( data() ) + pos;
	return *reinterpret_cast<Type*>( bytes );
}



template<typename Type>
inline Type& byte_span::front() const
{
	return at<Type>( 0 );
}



template<typename Type>
inline Type& byte_span::back() const
{
	return at<Type>( size() - sizeof( Type ) );
}



inline byte_span::value_type* byte_span::data() const
{
	return mData;
}



inline byte_span::iterator byte_span::begin() const
{
	return data();
}



inline byte_span::const_iterator byte_span::cbegin() const
{
	return begin();
}



inline byte_span::iterator byte_span::end() const
{
	return reinterpret_cast<uint8_t*>( data() ) + size();
}



inline byte_span::const_iterator byte_span::cend() const
{
	return end();
}



inline byte_span::reverse_iterator byte_span::rbegin() const
{
	return reverse_iterator( reinterpret_cast<void_alias*>( end() ) );
}



inline byte_span::const_reverse_iterator byte_span::crbegin() const
{
	return rbegin();
}



inline byte_span::reverse_iterator byte_span::rend() const
{
	return reverse_iterator( reinterpret_cast<void_alias*>( begin() ) );
}



inline byte_span::const_reverse_iterator byte_span::crend() const
{
	return rend();
}



inline bool byte_span::empty() const
{
	return size() == 0;
}



inline byte_span::size_type byte_span::size() const
{
	return mSize;
}



inline byte_span::size_type byte_span::length() const
{
	return size();
}



inline constexpr byte_span::size_type byte_span::max_size() const
{
	return rftl::numeric_limits<size_type>::max();
}



inline void byte_span::remove_prefix( size_type size )
{
	RF_ASSERT( size <= this->size() );
	mSize -= size;
	mData = reinterpret_cast<uint8_t*>( mData ) + size;
}



inline void byte_span::remove_suffix( size_type size )
{
	RF_ASSERT( size <= this->size() );
	mSize -= size;
}



inline byte_span byte_span::subspan( size_type pos ) const
{
	RF_ASSERT( pos <= size() );
	return byte_span( reinterpret_cast<uint8_t*>( begin() ) + pos, end() );
}



inline byte_span byte_span::subspan( size_type pos, size_type size ) const
{
	RF_ASSERT( pos <= this->size() );
	RF_ASSERT( pos + size <= this->size() );
	return byte_span(
		reinterpret_cast<uint8_t*>( begin() ) + pos,
		reinterpret_cast<uint8_t*>( begin() ) + size );
}



template<typename Type>
inline Type byte_span::extract_front()
{
	Type retVal = front<Type>();
	remove_prefix( sizeof( Type ) );
	return retVal;
}



template<typename Type>
inline Type byte_span::extract_back()
{
	Type retVal = back<Type>();
	remove_suffix( sizeof( Type ) );
	return retVal;
}



inline void* byte_span::mem_copy_to( void* dest, size_t size ) const
{
	RF_ASSERT( size == this->size() );
	return mem_copy_prefix_to( dest, size );
}



inline void* byte_span::mem_copy_prefix_to( void* dest, size_t size ) const
{
	RF_ASSERT( size <= this->size() );
	RF_ASSERT( dest != nullptr );
	void const* const src = data();
	RF_ASSERT( src != nullptr );
	RF_ASSERT( src != dest );
	rftl::memcpy( dest, src, size );
	return dest;
}



template<typename Container>
inline void* byte_span::mem_copy_to( Container& container ) const
{
	static_assert( sizeof( Container::value_type ) == 1 );
	typename Container::value_type* const data = container.data();
	size_t const size = container.size();
	return mem_copy_to( data, size );
}



template<typename Container>
inline void* byte_span::mem_copy_prefix_to( Container& container ) const
{
	static_assert( sizeof( typename Container::value_type ) == 1 );
	typename Container::value_type* const data = container.data();
	size_t const size = container.size();
	return mem_copy_prefix_to( data, size );
}

///////////////////////////////////////////////////////////////////////////////

inline bool operator==( byte_span const& lhs, byte_span const& rhs )
{
	return lhs.size() == rhs.size() && rftl::memcmp( lhs.data(), rhs.data(), lhs.size() ) == 0;
}



inline bool operator!=( byte_span const& lhs, byte_span const& rhs )
{
	return lhs.size() != rhs.size() || rftl::memcmp( lhs.data(), rhs.data(), lhs.size() ) != 0;
}

///////////////////////////////////////////////////////////////////////////////
}

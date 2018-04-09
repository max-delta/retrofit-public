#pragma once
#include "math_bytes.h"

#include "rftl/cstdint"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
constexpr T ReverseByteOrder( T const value )
{
	static_assert( false, "No matching primitive found" );
	return T();
}
template<> constexpr uint8_t ReverseByteOrder( uint8_t const value )
{
	return value;
}
template<> constexpr int8_t ReverseByteOrder( int8_t const value )
{
	return value;
}
template<> constexpr uint16_t ReverseByteOrder( uint16_t const value )
{
	using T = uint16_t;
	T const a = ( value >> 8 ) & 0xffu;
	T const b = ( value >> 0 ) & 0xffu;
	T const ret = static_cast<T>(
		( a << 0 ) |
		( b << 8 ) );
	return ret;
}
template<> constexpr int16_t ReverseByteOrder( int16_t const value )
{
	using T = int16_t;
	T const a = ( value >> 8 ) & 0xff;
	T const b = ( value >> 0 ) & 0xff;
	T const ret = static_cast<T>(
		( a << 0 ) |
		( b << 8 ) );
	return ret;
}
template<> constexpr uint32_t ReverseByteOrder( uint32_t const value )
{
	using T = uint32_t;
	T const a = ( value >> 24 ) & 0xffu;
	T const b = ( value >> 16 ) & 0xffu;
	T const c = ( value >> 8 ) & 0xffu;
	T const d = ( value >> 0 ) & 0xffu;
	T const ret = static_cast<T>(
		( a << 0 ) |
		( b << 8 ) |
		( c << 16 ) |
		( d << 24 ) );
	return ret;
}
template<> constexpr int32_t ReverseByteOrder( int32_t const value )
{
	using T = int32_t;
	T const a = ( value >> 24 ) & 0xff;
	T const b = ( value >> 16 ) & 0xff;
	T const c = ( value >> 8 ) & 0xff;
	T const d = ( value >> 0 ) & 0xff;
	T const ret = static_cast<T>(
		( a << 0 ) |
		( b << 8 ) |
		( c << 16 ) |
		( d << 24 ) );
	return ret;
}
template<> constexpr uint64_t ReverseByteOrder( uint64_t const value )
{
	using T = uint64_t;
	T const a = ( value >> 56 ) & 0xffu;
	T const b = ( value >> 48 ) & 0xffu;
	T const c = ( value >> 40 ) & 0xffu;
	T const d = ( value >> 32 ) & 0xffu;
	T const e = ( value >> 24 ) & 0xffu;
	T const f = ( value >> 16 ) & 0xffu;
	T const g = ( value >> 8 ) & 0xffu;
	T const h = ( value >> 0 ) & 0xffu;
	T const ret = static_cast<T>(
		( a << 0 ) |
		( b << 8 ) |
		( c << 16 ) |
		( d << 24 ) |
		( e << 32 ) |
		( f << 40 ) |
		( g << 48 ) |
		( h << 56 ) );
	return ret;
}
template<> constexpr int64_t ReverseByteOrder( int64_t const value )
{
	using T = int64_t;
	T const a = ( value >> 56 ) & 0xff;
	T const b = ( value >> 48 ) & 0xff;
	T const c = ( value >> 40 ) & 0xff;
	T const d = ( value >> 32 ) & 0xff;
	T const e = ( value >> 24 ) & 0xff;
	T const f = ( value >> 16 ) & 0xff;
	T const g = ( value >> 8 ) & 0xff;
	T const h = ( value >> 0 ) & 0xff;
	T const ret = static_cast<T>(
		( a << 0 ) |
		( b << 8 ) |
		( c << 16 ) |
		( d << 24 ) |
		( e << 32 ) |
		( f << 40 ) |
		( g << 48 ) |
		( h << 56 ) );
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include <stdint.h>
#include <type_traits>

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template <typename T>
class Vector2 final
{
	static_assert( std::is_signed<T>::value, "Vector2 must be signed" );
	static_assert( std::is_arithmetic<T>::value, "Vector2 must support arithmetic operations" );
public:
	Vector2();
	Vector2( T x, T y );
	T x;
	T y;
};

///////////////////////////////////////////////////////////////////////////////
}}

namespace RF { namespace math {
	typedef Vector2<  float> Vector2f;
	typedef Vector2< double> Vector2d;
	typedef Vector2< int8_t> Vector2i8;
	typedef Vector2<int16_t> Vector2i16;
	typedef Vector2<int32_t> Vector2i32;
	typedef Vector2<int64_t> Vector2i64;
}}
#ifndef __INTELLISENSE__
	// Intellisense in VS2015 has a check for blocking implicit instantiation
	//  ordering w.r.t. explicit instantiation, but appears to be completely
	//  broken
	extern template class RF::math::Vector2<  float>;
	extern template class RF::math::Vector2< double>;
	extern template class RF::math::Vector2< int8_t>;
	extern template class RF::math::Vector2<int16_t>;
	extern template class RF::math::Vector2<int32_t>;
	extern template class RF::math::Vector2<int64_t>;
#endif

#include "Vector2.inl"

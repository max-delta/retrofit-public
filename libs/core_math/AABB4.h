#pragma once
#include "core_math/Vector2.h"

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class AABB4 final
{
	static_assert( rftl::is_signed<T>::value, "AABB4 must be signed" );
	static_assert( rftl::is_arithmetic<T>::value, "AABB4 must support arithmetic operations" );

public:
	typedef T ElementType;


public:
	AABB4();
	AABB4( Vector2<T> topLeft, Vector2<T> bottomRight );
	AABB4( T left, T top, T right, T bottom );

	bool operator==( AABB4 const& rhs ) const;

	template<typename OtherT>
	operator AABB4<OtherT>() const;

	T const& Left() const;
	T const& Right() const;
	T const& Top() const;
	T const& Bottom() const;


public:
	Vector2<T> mTopLeft;
	Vector2<T> mBottomRight;
};

///////////////////////////////////////////////////////////////////////////////
}}

namespace RF { namespace math {
typedef AABB4<float> AABB4f;
typedef AABB4<double> AABB4d;
typedef AABB4<int8_t> AABB4i8;
typedef AABB4<int16_t> AABB4i16;
typedef AABB4<int32_t> AABB4i32;
typedef AABB4<int64_t> AABB4i64;
}}
#ifndef __INTELLISENSE__
// Intellisense in VS2015 has a check for blocking implicit instantiation
//  ordering w.r.t. explicit instantiation, but appears to be completely
//  broken
extern template class RF::math::AABB4<float>;
extern template class RF::math::AABB4<double>;
extern template class RF::math::AABB4<int8_t>;
extern template class RF::math::AABB4<int16_t>;
extern template class RF::math::AABB4<int32_t>;
extern template class RF::math::AABB4<int64_t>;
#endif

#include "AABB4.inl"

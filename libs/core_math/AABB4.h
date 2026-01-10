#pragma once
#include "core_math/Vector2.h"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Does not assume +/- behavior of X and Y w.r.t. directions
template<typename T>
class AABB4 final
{
	static_assert( rftl::is_signed<T>::value, "AABB4 must be signed" );
	static_assert( rftl::is_arithmetic<T>::value, "AABB4 must support arithmetic operations" );

public:
	using ElementType = T;


public:
	AABB4();
	AABB4( Vector2<T> topLeft, Vector2<T> bottomRight );
	AABB4( T left, T top, T right, T bottom );

	bool operator==( AABB4 const& rhs ) const;
	bool operator!=( AABB4 const& rhs ) const;

	AABB4 operator+( Vector2<T> const& rhs ) const;
	AABB4 operator-( Vector2<T> const& rhs ) const;
	AABB4& operator+=( Vector2<T> const& rhs );
	AABB4& operator-=( Vector2<T> const& rhs );

	template<typename OtherT>
	operator AABB4<OtherT>() const;

	T const& Left() const;
	T const& Right() const;
	T const& Top() const;
	T const& Bottom() const;

	T& Left();
	T& Right();
	T& Top();
	T& Bottom();

	T Width() const;
	T Height() const;
	Vector2<T> Dimensions() const;


public:
	Vector2<T> mTopLeft;
	Vector2<T> mBottomRight;
};

///////////////////////////////////////////////////////////////////////////////
}

// Explicitly instantiate and alias common types
namespace RF::math {
using AABB4f = AABB4<float>;
using AABB4d = AABB4<double>;
using AABB4i8 = AABB4<int8_t>;
using AABB4i16 = AABB4<int16_t>;
using AABB4i32 = AABB4<int32_t>;
using AABB4i64 = AABB4<int64_t>;
}
extern template class RF::math::AABB4<float>;
extern template class RF::math::AABB4<double>;
extern template class RF::math::AABB4<int8_t>;
extern template class RF::math::AABB4<int16_t>;
extern template class RF::math::AABB4<int32_t>;
extern template class RF::math::AABB4<int64_t>;

#include "AABB4.inl"

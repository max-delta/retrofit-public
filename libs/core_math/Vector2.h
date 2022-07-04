#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class Vector2 final
{
	static_assert( rftl::is_signed<T>::value, "Vector2 must be signed" );
	static_assert( rftl::is_arithmetic<T>::value, "Vector2 must support arithmetic operations" );

	//
	// Types
public:
	using ElementType = T;


	//
	// Public methods
public:
	constexpr Vector2();
	constexpr Vector2( T x, T y );

	template<typename OtherT>
	operator Vector2<OtherT>() const;

	constexpr bool operator==( Vector2 const& rhs ) const;
	constexpr bool operator!=( Vector2 const& rhs ) const;

	constexpr Vector2 operator-() const;

	constexpr Vector2 operator+( Vector2 const& rhs ) const;
	constexpr Vector2 operator-( Vector2 const& rhs ) const;
	constexpr Vector2& operator+=( Vector2 const& rhs );
	constexpr Vector2& operator-=( Vector2 const& rhs );

	constexpr Vector2 operator+( ElementType const& rhs ) const;
	constexpr Vector2 operator-( ElementType const& rhs ) const;
	constexpr Vector2 operator*( ElementType const& rhs ) const;
	constexpr Vector2& operator+=( ElementType const& rhs );
	constexpr Vector2& operator-=( ElementType const& rhs );
	constexpr Vector2& operator*=( ElementType const& rhs );


	//
	// Public data
public:
	T x;
	T y;
};

///////////////////////////////////////////////////////////////////////////////
}

// Explicitly instantiate and alias common types
namespace RF::math {
using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector2i8 = Vector2<int8_t>;
using Vector2i16 = Vector2<int16_t>;
using Vector2i32 = Vector2<int32_t>;
using Vector2i64 = Vector2<int64_t>;
}
extern template class RF::math::Vector2<float>;
extern template class RF::math::Vector2<double>;
extern template class RF::math::Vector2<int8_t>;
extern template class RF::math::Vector2<int16_t>;
extern template class RF::math::Vector2<int32_t>;
extern template class RF::math::Vector2<int64_t>;

#include "Vector2.inl"

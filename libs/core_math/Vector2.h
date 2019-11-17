#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF { namespace math {
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
	Vector2();
	Vector2( T x, T y );

	template<typename OtherT>
	operator Vector2<OtherT>() const;

	bool operator==( Vector2 const& rhs ) const;
	bool operator!=( Vector2 const& rhs ) const;

	Vector2 operator+( Vector2 const& rhs ) const;
	Vector2 operator-( Vector2 const& rhs ) const;
	Vector2& operator+=( Vector2 const& rhs );
	Vector2& operator-=( Vector2 const& rhs );

	Vector2 operator+( ElementType const& rhs ) const;
	Vector2 operator-( ElementType const& rhs ) const;
	Vector2 operator*( ElementType const& rhs ) const;
	Vector2& operator+=( ElementType const& rhs );
	Vector2& operator-=( ElementType const& rhs );
	Vector2& operator*=( ElementType const& rhs );


	//
	// Public data
public:
	T x;
	T y;
};

///////////////////////////////////////////////////////////////////////////////
}}

// Explicitly instantiate and alias common types
namespace RF { namespace math {
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;
typedef Vector2<int8_t> Vector2i8;
typedef Vector2<int16_t> Vector2i16;
typedef Vector2<int32_t> Vector2i32;
typedef Vector2<int64_t> Vector2i64;
}}
extern template class RF::math::Vector2<float>;
extern template class RF::math::Vector2<double>;
extern template class RF::math::Vector2<int8_t>;
extern template class RF::math::Vector2<int16_t>;
extern template class RF::math::Vector2<int32_t>;
extern template class RF::math::Vector2<int64_t>;

#include "Vector2.inl"

#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class Vector3 final
{
	static_assert( rftl::is_signed<T>::value, "Vector3 must be signed" );
	static_assert( rftl::is_arithmetic<T>::value, "Vector3 must support arithmetic operations" );

	//
	// Types
public:
	using ElementType = T;


	//
	// Public methods
public:
	Vector3();
	Vector3( T x, T y, T z );

	template<typename OtherT>
	operator Vector3<OtherT>() const;

	bool operator==( Vector3 const& rhs ) const;

	Vector3 operator+( Vector3 const& rhs ) const;
	Vector3 operator-( Vector3 const& rhs ) const;
	Vector3& operator+=( Vector3 const& rhs );
	Vector3& operator-=( Vector3 const& rhs );

	Vector3 operator+( ElementType const& rhs ) const;
	Vector3 operator-( ElementType const& rhs ) const;
	Vector3 operator*( ElementType const& rhs ) const;
	Vector3& operator+=( ElementType const& rhs );
	Vector3& operator-=( ElementType const& rhs );
	Vector3& operator*=( ElementType const& rhs );


	//
	// Public data
public:
	T x;
	T y;
	T z;
};

///////////////////////////////////////////////////////////////////////////////
}

// Explicitly instantiate and alias common types
namespace RF::math {
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vector3i8 = Vector3<int8_t>;
using Vector3i16 = Vector3<int16_t>;
using Vector3i32 = Vector3<int32_t>;
using Vector3i64 = Vector3<int64_t>;
}
extern template class RF::math::Vector3<float>;
extern template class RF::math::Vector3<double>;
extern template class RF::math::Vector3<int8_t>;
extern template class RF::math::Vector3<int16_t>;
extern template class RF::math::Vector3<int32_t>;
extern template class RF::math::Vector3<int64_t>;

#include "Vector3.inl"

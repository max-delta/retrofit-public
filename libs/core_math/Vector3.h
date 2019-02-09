#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF { namespace math {
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
}}

namespace RF { namespace math {
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
typedef Vector3<int8_t> Vector3i8;
typedef Vector3<int16_t> Vector3i16;
typedef Vector3<int32_t> Vector3i32;
typedef Vector3<int64_t> Vector3i64;
}}
#ifndef __INTELLISENSE__
// Intellisense in VS2015 has a check for blocking implicit instantiation
//  ordering w.r.t. explicit instantiation, but appears to be completely
//  broken
extern template class RF::math::Vector3<float>;
extern template class RF::math::Vector3<double>;
extern template class RF::math::Vector3<int8_t>;
extern template class RF::math::Vector3<int16_t>;
extern template class RF::math::Vector3<int32_t>;
extern template class RF::math::Vector3<int64_t>;
#endif

#include "Vector3.inl"

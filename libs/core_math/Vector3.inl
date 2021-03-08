#pragma once
#include "Vector3.h"

#include "core_math/math_compare.h"
#include "core_math/math_casts.h"
#include "core_math/NumericTraits.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
inline Vector3<T>::Vector3()
	: x( NumericTraits<T>::Empty() )
	, y( NumericTraits<T>::Empty() )
	, z( NumericTraits<T>::Empty() )
{
	//
}



template<typename T>
inline Vector3<T>::Vector3( T x, T y, T z )
	: x( x )
	, y( y )
	, z( z )
{
	//
}



template<typename T>
template<typename OtherT>
inline Vector3<T>::operator Vector3<OtherT>() const
{
	return Vector3<OtherT>(
		math::real_cast<OtherT>( x ),
		math::real_cast<OtherT>( y ),
		math::real_cast<OtherT>( z ) );
}



template<typename T>
inline bool Vector3<T>::operator==( Vector3 const& rhs ) const
{
	return Equals( x, rhs.x ) && Equals( y, rhs.y ) && Equals( z, rhs.z );
}



template<typename T>
inline Vector3<T> Vector3<T>::operator+( Vector3 const& rhs ) const
{
	return Vector3( x + rhs.x, y + rhs.y, z + rhs.z );
}



template<typename T>
inline Vector3<T> Vector3<T>::operator-( Vector3 const& rhs ) const
{
	return Vector3( x - rhs.x, y - rhs.y, z - rhs.z );
}



template<typename T>
inline Vector3<T>& Vector3<T>::operator+=( Vector3 const& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}



template<typename T>
inline Vector3<T>& Vector3<T>::operator-=( Vector3 const& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}



template<typename T>
inline Vector3<T> Vector3<T>::operator+( ElementType const& rhs ) const
{
	return Vector3( x + rhs, y + rhs, z + rhs );
}



template<typename T>
inline Vector3<T> Vector3<T>::operator-( ElementType const& rhs ) const
{
	return Vector3( x - rhs, y - rhs, z - rhs );
}



template<typename T>
inline Vector3<T> Vector3<T>::operator*( ElementType const& rhs ) const
{
	return Vector3( x * rhs, y * rhs, z * rhs );
}



template<typename T>
inline Vector3<T>& Vector3<T>::operator+=( ElementType const& rhs )
{
	x += rhs;
	y += rhs;
	z += rhs;
	return *this;
}



template<typename T>
inline Vector3<T>& Vector3<T>::operator-=( ElementType const& rhs )
{
	x -= rhs;
	y -= rhs;
	z -= rhs;
	return *this;
}



template<typename T>
inline Vector3<T>& Vector3<T>::operator*=( ElementType const& rhs )
{
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "Vector2.h"

#include "core_math/math_compare.h"
#include "core_math/math_casts.h"
#include "core_math/NumericTraits.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
inline Vector2<T>::Vector2()
	: x( NumericTraits<T>::Empty() )
	, y( NumericTraits<T>::Empty() )
{
	//
}



template<typename T>
inline Vector2<T>::Vector2( T x, T y )
	: x( x )
	, y( y )
{
	//
}



template<typename T>
template<typename OtherT>
inline Vector2<T>::operator Vector2<OtherT>() const
{
	return Vector2<OtherT>(
		math::real_cast<OtherT>( x ),
		math::real_cast<OtherT>( y ) );
}



template<typename T>
inline bool Vector2<T>::operator==( Vector2 const& rhs ) const
{
	return Equals( x, rhs.x ) && Equals( y, rhs.y );
}



template<typename T>
inline bool Vector2<T>::operator!=( Vector2 const& rhs ) const
{
	return this->operator==( rhs ) == false;
}



template<typename T>
inline Vector2<T> Vector2<T>::operator+( Vector2 const& rhs ) const
{
	return Vector2( x + rhs.x, y + rhs.y );
}



template<typename T>
inline Vector2<T> Vector2<T>::operator-( Vector2 const& rhs ) const
{
	return Vector2( x - rhs.x, y - rhs.y );
}



template<typename T>
inline Vector2<T>& Vector2<T>::operator+=( Vector2 const& rhs )
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}



template<typename T>
inline Vector2<T>& Vector2<T>::operator-=( Vector2 const& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}



template<typename T>
inline Vector2<T> Vector2<T>::operator+( ElementType const& rhs ) const
{
	return Vector2( x + rhs, y + rhs );
}



template<typename T>
inline Vector2<T> Vector2<T>::operator-( ElementType const& rhs ) const
{
	return Vector2( x - rhs, y - rhs );
}



template<typename T>
inline Vector2<T> Vector2<T>::operator*( ElementType const& rhs ) const
{
	return Vector2( x * rhs, y * rhs );
}



template<typename T>
inline Vector2<T>& Vector2<T>::operator+=( ElementType const& rhs )
{
	x += rhs;
	y += rhs;
	return *this;
}



template<typename T>
inline Vector2<T>& Vector2<T>::operator-=( ElementType const& rhs )
{
	x -= rhs;
	y -= rhs;
	return *this;
}



template<typename T>
inline Vector2<T>& Vector2<T>::operator*=( ElementType const& rhs )
{
	x *= rhs;
	y *= rhs;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}}

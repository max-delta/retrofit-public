#pragma once
#include "AABB4.h"

#include "core_math/math_clamps.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
inline RF::math::AABB4<T>::AABB4()
{
	//
}



template<typename T>
inline AABB4<T>::AABB4( Vector2<T> topLeft, Vector2<T> bottomRight )
	: mTopLeft( topLeft )
	, mBottomRight( bottomRight )
{
	//
}



template<typename T>
inline AABB4<T>::AABB4( T left, T top, T right, T bottom )
	: mTopLeft( left, top )
	, mBottomRight( right, bottom )
{
	//
}



template<typename T>
inline bool AABB4<T>::operator==( AABB4 const& rhs ) const
{
	return mTopLeft == rhs.mTopLeft && mBottomRight == rhs.mBottomRight;
}



template<typename T>
inline bool AABB4<T>::operator!=( AABB4 const& rhs ) const
{
	return this->operator==( rhs ) == false;
}



template<typename T>
inline AABB4<T> AABB4<T>::operator+( Vector2<T> const& rhs ) const
{
	return AABB4( mTopLeft + rhs, mBottomRight + rhs );
}



template<typename T>
inline AABB4<T> AABB4<T>::operator-( Vector2<T> const& rhs ) const
{
	return AABB4( mTopLeft - rhs, mBottomRight - rhs );
}



template<typename T>
inline AABB4<T>& AABB4<T>::operator+=( Vector2<T> const& rhs )
{
	mTopLeft += rhs;
	mBottomRight += rhs;
	return *this;
}



template<typename T>
inline AABB4<T>& AABB4<T>::operator-=( Vector2<T> const& rhs )
{
	mTopLeft -= rhs;
	mBottomRight -= rhs;
	return *this;
}



template<typename T>
template<typename OtherT>
inline AABB4<T>::operator AABB4<OtherT>() const
{
	return AABB4<OtherT>(
		static_cast<OtherT>( mTopLeft ),
		static_cast<OtherT>( mBottomRight ) );
}



template<typename T>
inline T const& AABB4<T>::Left() const
{
	return mTopLeft.x;
}



template<typename T>
inline T const& AABB4<T>::Right() const
{
	return mBottomRight.x;
}



template<typename T>
inline T const& AABB4<T>::Top() const
{
	return mTopLeft.y;
}



template<typename T>
inline T const& AABB4<T>::Bottom() const
{
	return mBottomRight.y;
}



template<typename T>
inline T& AABB4<T>::Left()
{
	return mTopLeft.x;
}



template<typename T>
inline T& AABB4<T>::Right()
{
	return mBottomRight.x;
}



template<typename T>
inline T& AABB4<T>::Top()
{
	return mTopLeft.y;
}



template<typename T>
inline T& AABB4<T>::Bottom()
{
	return mBottomRight.y;
}



template<typename T>
inline T AABB4<T>::Width() const
{
	return Abs<T>( Right() - Left() );
}



template<typename T>
inline T AABB4<T>::Height() const
{
	return Abs<T>( Bottom() - Top() );
}

///////////////////////////////////////////////////////////////////////////////
}

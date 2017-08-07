#pragma once
#include "Vector2.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
inline RF::math::Vector2<T>::Vector2()
	: x()
	, y()
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

///////////////////////////////////////////////////////////////////////////////
}}

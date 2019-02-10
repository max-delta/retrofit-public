#pragma once
#include "Lerp.h"

#include "rftl/type_traits"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE, typename SCALAR>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, SCALAR const& t )
{
	return vt0 + t * ( vt1 - vt0 );
}



template<typename TYPE>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, float const& t )
{
	return static_cast<TYPE>( ( 1.f - t ) * vt0 ) + static_cast<TYPE>( t * vt1 );
}



template<typename TYPE>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, double const& t )
{
	return static_cast<TYPE>( ( 1.0 - t ) * vt0 ) + static_cast<TYPE>( t * vt1 );
}



template<typename TYPE>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, long double const& t )
{
	return static_cast<TYPE>( ( 1.0l - t ) * vt0 ) + static_cast<TYPE>( t * vt1 );
}

///////////////////////////////////////////////////////////////////////////////
}}

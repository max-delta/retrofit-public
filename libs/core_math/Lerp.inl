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



template<typename RESULT, typename TYPE>
constexpr RESULT Unlerp( TYPE const& vt0, TYPE const& vt1, TYPE const& v )
{
	// NOTE: Difference type may not be the same as operand type
	// EXAMPLE: chrono::time_point vs chrono::duration
	using DiffT = decltype( vt1 - vt0 );

	DiffT const zero = vt0 - vt0;
	DiffT const range = vt1 - vt0;
	DiffT const offset = v - vt0;

	bool const valid = range > zero;

	// NOTE: Convert to result first, under the assumption that the common
	//  cases would result in a lossy integer division if it wasn't converted
	return valid ? static_cast<RESULT>( offset ) / static_cast<RESULT>( range ) : RESULT{};
}



template<typename OUTTYPE, typename INTYPE, typename RATIOTYPE>
constexpr OUTTYPE Rescale( OUTTYPE const& outMin, OUTTYPE const& outMax, INTYPE const& inMin, INTYPE const& inMax, INTYPE const& inVal )
{
	RATIOTYPE const t = Unlerp<RATIOTYPE>( inMin, inMax, inVal );
	return Lerp( outMin, outMax, t );
}

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// This lerp prefers the (vt1 - vt0) option
template<typename TYPE, typename SCALAR>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, SCALAR const& t );

// This lerp prefers the (1-t) option
template<typename TYPE>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, float const& t );
template<typename TYPE>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, double const& t );
template<typename TYPE>
constexpr TYPE Lerp( TYPE const& vt0, TYPE const& vt1, long double const& t );

template<typename RESULT, typename TYPE>
constexpr RESULT Unlerp( TYPE const& vt0, TYPE const& vt1, TYPE const& v );

template<typename OUTTYPE, typename INTYPE, typename RATIOTYPE = float>
constexpr OUTTYPE Rescale( OUTTYPE const& outMin, OUTTYPE const& outMax, INTYPE const& inMin, INTYPE const& inMax, INTYPE const& inVal );

///////////////////////////////////////////////////////////////////////////////
}

#include "Lerp.inl"

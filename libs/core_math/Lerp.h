#pragma once


namespace RF { namespace math {
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

///////////////////////////////////////////////////////////////////////////////
}}

#include "Lerp.inl"

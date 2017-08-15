#pragma once


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr TYPE const& Min( TYPE const& lhs, TYPE const& rhs );

template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs );

template<typename TYPE>
constexpr TYPE const& Clamp( TYPE const& min, TYPE const& value, TYPE const& max );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_clamps.inl"

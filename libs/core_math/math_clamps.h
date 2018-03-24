#pragma once


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr TYPE const& Min( TYPE const& lhs, TYPE const& rhs );

template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs );

template<typename TYPE>
constexpr TYPE const& Clamp( TYPE const& min, TYPE const& value, TYPE const& max );

template<typename TYPE>
constexpr TYPE SnapNearest( TYPE const& value, TYPE const& step );

template<typename TYPE>
constexpr TYPE SnapHighest( TYPE const& value, TYPE const& step );

template<typename TYPE>
constexpr TYPE SnapLowest( TYPE const& value, TYPE const& step );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_clamps.inl"

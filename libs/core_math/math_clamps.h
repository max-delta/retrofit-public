#pragma once
#include "rftl/cstdint"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr TYPE const& Min( TYPE const& lhs, TYPE const& rhs );
template<typename TYPE>
constexpr TYPE const& Min( TYPE const& lhs, TYPE const& rhs, TYPE const& additional... );

template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs );
template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs, TYPE const& additional... );

template<typename TYPE>
constexpr TYPE const& Clamp( TYPE const& min, TYPE const& value, TYPE const& max );

template<typename TYPE>
constexpr TYPE SnapNearest( TYPE const& value, TYPE const& step );

template<typename TYPE>
constexpr TYPE SnapHighest( TYPE const& value, TYPE const& step );

template<typename TYPE>
constexpr TYPE SnapLowest( TYPE const& value, TYPE const& step );

template<typename BASE, typename OFFSET>
constexpr BASE WrapPositiveOffset( BASE const& value, BASE const& mod, OFFSET const& offset );

template<typename TYPE>
constexpr TYPE Abs( TYPE const& value );

///////////////////////////////////////////////////////////////////////////////
}

#include "math_clamps.inl"

#pragma once
#include "math_clamps.h"

#include "core/macros.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr TYPE const& Min(TYPE const& lhs, TYPE const& rhs)
{
	return lhs < rhs ? lhs : rhs;
}



template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs )
{
	return lhs > rhs ? lhs : rhs;
}



template<typename TYPE>
constexpr TYPE const& Clamp( TYPE const& min, TYPE const& value, TYPE const& max )
{
	RF_ASSERT( min <= max );
	return Min( Max( min, value ), max );
}

///////////////////////////////////////////////////////////////////////////////
}}

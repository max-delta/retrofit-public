#pragma once
#include "math_compare.h"

#include <limits>


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////
namespace internal {
	constexpr float fabs( float val )
	{
		return val < 0 ? val * -1 : val;
	}

	constexpr double abs( double val )
	{
		return val < 0 ? val * -1 : val;
	}
}
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr bool Equals( TYPE const& lhs, TYPE const& rhs )
{
	return lhs == rhs;
}



template<>
constexpr bool Equals<float>( float const& lhs, float const& rhs )
{
	return
		lhs == rhs ||
		internal::fabs( lhs - rhs ) < std::numeric_limits<float>::epsilon() * 10 ||
		( lhs > rhs && lhs / rhs < 1.000001f ) ||
		( rhs > lhs && rhs / lhs < 1.000001f );
}



template<>
constexpr bool Equals<double>( double const& lhs, double const& rhs )
{
	return
		lhs == rhs ||
		internal::abs( lhs - rhs ) < std::numeric_limits<double>::epsilon() * 10 ||
		( lhs > rhs && lhs / rhs < 1.00000000001 ) ||
		( rhs > lhs && rhs / lhs < 1.00000000001 );
}

///////////////////////////////////////////////////////////////////////////////
}}

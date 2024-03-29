#pragma once
#include "math_compare.h"

#include "core/compiler.h"

#include "rftl/limits"


namespace RF::math {
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
	bool const lhsIsZero = internal::fabs( lhs ) < rftl::numeric_limits<float>::epsilon() * 10;
	bool const rhsIsZero = internal::fabs( rhs ) < rftl::numeric_limits<float>::epsilon() * 10;
	bool const deltaIsZero = internal::fabs( lhs - rhs ) < rftl::numeric_limits<float>::epsilon() * 10;
	return deltaIsZero ||
		( lhsIsZero && rhsIsZero ) ||
		( rhsIsZero == false && lhs > rhs && lhs / rhs < 1.000001f ) ||
		( lhsIsZero == false && rhs > lhs && rhs / lhs < 1.000001f );
}



template<>
constexpr bool Equals<double>( double const& lhs, double const& rhs )
{
	bool const lhsIsZero = internal::abs( lhs ) < rftl::numeric_limits<double>::epsilon() * 10;
	bool const rhsIsZero = internal::abs( rhs ) < rftl::numeric_limits<double>::epsilon() * 10;
	bool const deltaIsZero = internal::abs( lhs - rhs ) < rftl::numeric_limits<double>::epsilon() * 10;
	return deltaIsZero ||
		( lhsIsZero && rhsIsZero ) ||
		( rhsIsZero == false && lhs > rhs && lhs / rhs < 1.00000000001 ) ||
		( lhsIsZero == false && rhs > lhs && rhs / lhs < 1.00000000001 );
}



constexpr bool UncheckedEquals( float const& lhs, float const& rhs )
{
	RF_CLANG_PUSH();
	RF_CLANG_IGNORE( "-Wfloat-equal" );
	return lhs == rhs;
	RF_CLANG_POP();
}



constexpr bool UncheckedEquals( double const& lhs, double const& rhs )
{
	RF_CLANG_PUSH();
	RF_CLANG_IGNORE( "-Wfloat-equal" );
	return lhs == rhs;
	RF_CLANG_POP();
}



constexpr bool UncheckedEquals( long double const& lhs, long double const& rhs )
{
	RF_CLANG_PUSH();
	RF_CLANG_IGNORE( "-Wfloat-equal" );
	return lhs == rhs;
	RF_CLANG_POP();
}



template<typename TYPE>
constexpr bool IsWithin( TYPE const& lhs, TYPE const& delta, TYPE const& rhs )
{
	if( lhs <= rhs && lhs + delta >= rhs )
	{
		return true;
	}

	if( lhs >= rhs && lhs + delta >= rhs )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
}

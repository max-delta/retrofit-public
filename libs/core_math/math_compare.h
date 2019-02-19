#pragma once


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr bool Equals( TYPE const& lhs, TYPE const& rhs );

template<>
constexpr bool Equals<float>( float const& lhs, float const& rhs );

template<>
constexpr bool Equals<double>( double const& lhs, double const& rhs );


constexpr bool UncheckedEquals( float const& lhs, float const& rhs );
constexpr bool UncheckedEquals( double const& lhs, double const& rhs );
constexpr bool UncheckedEquals( long double const& lhs, long double const& rhs );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_compare.inl"

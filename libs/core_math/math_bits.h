#pragma once

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
constexpr bool HasOnly1BitSet( T const value );

template<typename T>
constexpr size_t GetOnesIndexOfHighestBit( T const value );
template<typename T>
constexpr size_t GetZerosIndexOfHighestBit( T const value );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_bits.inl"

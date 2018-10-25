#pragma once

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
constexpr T GetAllBitsSet();

template<typename T>
constexpr bool HasOnly1BitSet( T const value );

template<typename T>
constexpr size_t GetOnesIndexOfHighestBit( T const value );
template<typename T>
constexpr size_t GetZerosIndexOfHighestBit( T const value );

bool IsBitSet( void const* root, size_t offsetInBits );
void SetBit( void* root, size_t offsetInBits );
void ClearBit( void* root, size_t offsetInBits );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_bits.inl"

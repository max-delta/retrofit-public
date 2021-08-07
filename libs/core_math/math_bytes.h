#pragma once
#include "core/compiler.h"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
constexpr T ReverseByteOrder( T const value );

template<typename T>
constexpr T FromPlatformToBigEndian( T const value );
template<typename T>
constexpr T FromBigEndianToPlatform( T const value );

template<typename T>
constexpr T FromPlatformToLittleEndian( T const value );
template<typename T>
constexpr T FromLittleEndianToPlatform( T const value );

///////////////////////////////////////////////////////////////////////////////
}

#include "math_bytes.inl"

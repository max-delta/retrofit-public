#pragma once
#include "core/compiler.h"

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
constexpr T ReverseByteOrder( T const value );

template<typename T>
constexpr T FromPlatformToBigEndian( T const value );
template<typename T>
constexpr T FromBigEndianToPlatform( T const value );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_bytes.inl"

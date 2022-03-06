#pragma once

#include "rftl/cstdint"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class SoftFloat;

///////////////////////////////////////////////////////////////////////////////

template<>
class SoftFloat<float>
{
public:
	using Value = float;
	using Rep = uint32_t;
	using Exponent = int32_t;

public:
	// ... * -1^<SIGN> * ...
	static Rep SignBit( Value const& val );

	// ... * 2^(<EXP>-127) * ...
	static Rep ExponentBits( Value const& val );

	// ... * (1+FRAC) * ...
	static Rep FractionalBits( Value const& val );

	static bool IsNegative( Value const& val );
	static Exponent ExponentValue( Value const& val );

	static bool IsDenormalized( Value const& val );
	static bool IsInfinity( Value const& val );
	static bool IsNaN( Value const& val );
	static bool IsQuietNaN( Value const& val );
	static bool IsSignalingNaN( Value const& val );
};

///////////////////////////////////////////////////////////////////////////////
}

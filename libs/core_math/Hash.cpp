#include "stdafx.h"

#include "Hash.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

HashVal64 StableHashBytes( void const* buffer, size_t length )
{
	// Don't change the algorithm! Code may take dependencies on this behavior

	// FNV-1a hash
	constexpr HashVal64 kFNV_offset_basis = 14695981039346656037ull;
	constexpr HashVal64 kFNV_prime = 1099511628211ull;
	HashVal64 retVal = kFNV_offset_basis;
	for( size_t i = 0; i < length; ++i )
	{
		retVal ^= static_cast<HashVal64>( reinterpret_cast<uint8_t const*>( buffer )[i] );
		retVal *= kFNV_prime;
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

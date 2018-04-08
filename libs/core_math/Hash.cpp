#include "stdafx.h"

#include "Hash.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

HashVal64 DirectHash::operator() ( HashVal64 const& key ) const
{
	return key;
}

///////////////////////////////////////////////////////////////////////////////

HashVal64 NullTerminatedStringHash::operator()( char const * const buffer ) const
{
	if( buffer == nullptr )
	{
		return StableHashBytes( nullptr, 0 );
	}

	size_t length = 0;
	while( buffer[length] != '\0' )
	{
		length++;
	}

	return StableHashBytes( buffer, length );
}

///////////////////////////////////////////////////////////////////////////////

HashVal64 StableHashBytes( rftl::nullptr_t, size_t )
{
	// Reduced from non-null case
	constexpr HashVal64 kFNV_offset_basis = 14695981039346656037ull;
	return kFNV_offset_basis;
}



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

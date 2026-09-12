#include "stdafx.h"

#include "Hash.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

HashVal64 StableHashBytes( rftl::nullptr_t, size_t )
{
	// Reduced from non-null case
	return details::kFNV_offset_basis;
}



HashVal64 StableHashBytes( void const* buffer, size_t length )
{
	// Don't change the algorithm! Code may take dependencies on this behavior

	// Fowler/Noll/Vo FNV-1a 64-bit hash
	HashVal64 retVal = details::kFNV_offset_basis;
	for( size_t i = 0; i < length; ++i )
	{
		retVal ^= broaden_cast<HashVal64>( reinterpret_cast<uint8_t const*>( buffer )[i] );
		retVal *= details::kFNV_prime;
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

HashVal64 DirectHash::operator()( HashVal64 const& key ) const
{
	return key;
}

///////////////////////////////////////////////////////////////////////////////

HashVal64 NullTerminatedStringHash::operator()( char const* const buffer ) const
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
}

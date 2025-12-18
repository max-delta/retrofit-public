#pragma once

#include "core_math/MathFwd.h"

#include "core/meta/SafeCasts.h"

#include "rftl/cstddef"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// This hash function will not change based on platform
HashVal64 StableHashBytes( rftl::nullptr_t, size_t );
HashVal64 StableHashBytes( void const* buffer, size_t length );

// For constexpr string hashing, avoid using in non-constexpr scenarios
#define RF_HASH_FROM_STRING_LITERAL( LITERAL ) \
	RF_MSVC_INLINE_SUPPRESS( 4307 ) \
	::RF::math::details::ConstStableHashString( "" LITERAL "" )

struct DirectHash
{
	HashVal64 operator()( HashVal64 const& key ) const;
};

struct NullTerminatedStringHash
{
	HashVal64 operator()( char const* const buffer ) const;
};

template<typename T>
struct RawBytesHash
{
	HashVal64 operator()( T const& value ) const
	{
		return StableHashBytes( &value, sizeof( T ) );
	}
};

template<typename T1, typename T2, typename H1, typename H2>
struct PairHash
{
	HashVal64 operator()( rftl::pair<T1, T2> const& value ) const
	{
		HashVal64 const hash1 = H1()( value.first );
		HashVal64 const hash2 = H2()( value.second );
		return hash1 ^ hash2;
	}
};

template<typename T, typename H>
struct SequenceHash
{
	HashVal64 operator()( T const& value ) const
	{
		HashVal64 retVal = 0;
		for( auto const& elem : value )
		{
			// Golden ratio (1.640531527) as negated digit sequence
			// NOTE: This is pretty close to what Boost does, but that doesn't
			//  mean it's any good, Boost does lots of dumb stuff
			// NOTE: This is using a 32-bit salt, whereas Linux uses a 64-bit
			//  multiplier, but for a whole different formula and use-case
			// TODO: Find some academic research or something on this, maybe
			//  replace it with something better
			// SEE: boost::hash_combine<...>(...)
			// SEE: https://lkml.org/lkml/2016/4/29/838
			static constexpr uint32_t kSalt = 0x9e3779b9ul;
			retVal ^= H()( elem ) + kSalt + ( retVal << 6 ) + ( retVal >> 3 );
		}
		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
namespace details {

// SEE: StableHashBytes(...) internals
inline constexpr HashVal64 ConstStableHashString( char const* nullTerminatedString, HashVal64 rollingValue = 0xcbf29ce484222325ull )
{
	return ( nullTerminatedString[0] == '\0' ) ?
		rollingValue :
		ConstStableHashString(
			&nullTerminatedString[1],
			( rollingValue ^
				broaden_cast<HashVal64>( static_cast<unsigned char const>( nullTerminatedString[0] ) ) ) *
				0x100000001b3 );
}

}
///////////////////////////////////////////////////////////////////////////////
}

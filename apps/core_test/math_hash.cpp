#include "stdafx.h"

#include "core_math/Hash.h"
#include "rftl/limits"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( Hash, DirectHasher )
{
	DirectHash hasher{};
	ASSERT_EQ( hasher( 0 ), 0 );
	ASSERT_EQ( hasher( 1 ), 1 );
	ASSERT_EQ( hasher( 2 ), 2 );
	ASSERT_EQ( hasher( 3 ), 3 );
	ASSERT_EQ( hasher( rftl::numeric_limits<HashVal64>::max() ), rftl::numeric_limits<HashVal64>::max() );
}



TEST( Hash, NullTerminatedStringHasher )
{
	// This hashing function better not change! Strings hashes need to be able
	//  to be relied on across platforms and over time
	NullTerminatedStringHash hasher{};
	ASSERT_EQ( hasher( nullptr ), 14695981039346656037ull );
	ASSERT_EQ( hasher( "" ), 14695981039346656037ull ); // NOTE: Same as null
	ASSERT_EQ( hasher( "\0" ), 14695981039346656037ull ); // NOTE: Same as empty
	ASSERT_EQ( hasher( "\1" ), 12638152016183539244ull );
	ASSERT_EQ( hasher( "abc" ), 16654208175385433931ull );
}



TEST( Hash, StableHashBytes )
{
	// This hashing function better not change! The stable hash should be able
	//  to be relied on across platforms and over time
	ASSERT_EQ( StableHashBytes( nullptr, 0 ), 14695981039346656037ull );
	ASSERT_EQ( StableHashBytes( "\0", 1 ), 12638153115695167455ull );
	ASSERT_EQ( StableHashBytes( "\1", 1 ), 12638152016183539244ull );
	ASSERT_EQ( StableHashBytes( "abc", 3 ), 16654208175385433931ull );
}



TEST( Hash, HashCollisions )
{
	// Like all hashing algorthms, collisions exist, so make sure your code has
	//  a story for how to handle these collisions
	// NOTE: STL containers that use hashing will perform an equality check on
	//  the underlying data if hashes match, which is a common solution to both
	//  check for, and handle a collision

	// These are the shortest printable ascii strings that collide at zero
	{
		NullTerminatedStringHash hasher{};
		ASSERT_EQ( hasher( "!0IC=VloaY" ), 0 );
		ASSERT_EQ( hasher( "QvXtM>@Fp%" ), 0 );
		ASSERT_EQ( hasher( "=. hx\"iX<;" ), 0 );
		ASSERT_EQ( hasher( "_\"kWk=-v$c" ), 0 );
	}
}



TEST( Hash, ConstexprLiteralHash )
{
	NullTerminatedStringHash hasher{};

	// Can be used in constexpr values
	static constexpr HashVal64 k1 = RF_HASH_FROM_STRING_LITERAL( "" );
	ASSERT_EQ( hasher( "" ), k1 );
	static constexpr HashVal64 k2 = RF_HASH_FROM_STRING_LITERAL( "\0" );
	ASSERT_EQ( hasher( "\0" ), k2 );
	static constexpr HashVal64 k3 = RF_HASH_FROM_STRING_LITERAL( "\1" );
	ASSERT_EQ( hasher( "\1" ), k3 );
	static constexpr HashVal64 k4 = RF_HASH_FROM_STRING_LITERAL( "abc" );
	ASSERT_EQ( hasher( "abc" ), k4 );

	switch( hasher( "test" ) )
	{
		// Can be used in switch statements
		case RF_HASH_FROM_STRING_LITERAL( "test" ):
			ASSERT_TRUE( true );
			break;
		default:
			ASSERT_FALSE( true );
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

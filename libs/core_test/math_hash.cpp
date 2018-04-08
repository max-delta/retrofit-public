#include "stdafx.h"

#include "core_math/hash.h"
#include "rftl/limits"


namespace RF { namespace math {
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



TEST(Hash, StableHashBytes)
{
	// This hashing function better not change! The stable hash should be able
	//  to be relied on across platforms and over time
	ASSERT_EQ( StableHashBytes( nullptr, 0 ), 14695981039346656037ull );
	ASSERT_EQ( StableHashBytes( "\0", 1 ), 12638153115695167455ull );
	ASSERT_EQ( StableHashBytes( "\1", 1 ), 12638152016183539244ull );
	ASSERT_EQ( StableHashBytes( "abc", 3 ), 16654208175385433931ull );
}

///////////////////////////////////////////////////////////////////////////////
}}

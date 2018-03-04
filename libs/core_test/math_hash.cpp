#include "stdafx.h"

#include "core_math/hash.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

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

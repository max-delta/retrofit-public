#include "stdafx.h"

#include "core_math/Rand.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( Rand, StableLCG )
{
	// This random function better not change! Random behavior needs to be able
	//  to be relied on across platforms and over time
	ASSERT_EQ( StableRandLCG( 0 ), 12345 );
	ASSERT_EQ( StableRandLCG( 1 ), 60616 );
}



TEST( Rand, HashSeed )
{
	// This hashing function better not change!
	RF_TODO_ANNOTATION( "This hashing function is exceedingly trivial, is that still okay?" );
	ASSERT_EQ( GetSeedFromHash( 0 ), 0 );
	ASSERT_EQ( GetSeedFromHash( 1 ), 1 );
	ASSERT_EQ( GetSeedFromHash( 0xFFFF'FFFF ), 0xFFFF'FFFF );
	ASSERT_EQ( GetSeedFromHash( 0xFFFF'FFFF'FFFF'FFFF ), 0xFFFF'FFFE );
	ASSERT_EQ( GetSeedFromHash( 0x1234'FFFF'1234'FFFF ), 0x2469'FFFE );
}

///////////////////////////////////////////////////////////////////////////////
}

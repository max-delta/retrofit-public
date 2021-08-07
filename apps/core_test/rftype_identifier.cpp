#include "stdafx.h"

#include "core_rftype/Identifier.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

TEST( RFType, Identifier )
{
	// Must have some characters
	ASSERT_FALSE( IsValidIdentifier( "" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "" ), 0 );

	// Can't start with numbers
	ASSERT_FALSE( IsValidIdentifier( "0abc" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "0abc" ), 0 );
	ASSERT_TRUE( IsValidIdentifier( "abc0" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "abc0" ), 4 );

	// Valid characters
	ASSERT_TRUE( IsValidIdentifier( "abcdefghijklmnopqrstuvwxyz" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "abcdefghijklmnopqrstuvwxyz" ), 26 );
	ASSERT_TRUE( IsValidIdentifier( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ), 26 );
	ASSERT_TRUE( IsValidIdentifier( "_" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "_" ), 1 );
	ASSERT_TRUE( IsValidIdentifier( "_0123456789" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "_0123456789" ), 11 );

	// Some inalid characters
	ASSERT_FALSE( IsValidIdentifier( "." ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "." ), 0 );
	ASSERT_FALSE( IsValidIdentifier( ":" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( ":" ), 0 );
	ASSERT_FALSE( IsValidIdentifier( "/" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "/" ), 0 );
	ASSERT_FALSE( IsValidIdentifier( "\\" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "\\" ), 0 );
	ASSERT_FALSE( IsValidIdentifier( "$" ) );
	ASSERT_EQ( GetInvalidIdentifierCharacterIndex( "$" ), 0 );
}

///////////////////////////////////////////////////////////////////////////////
}

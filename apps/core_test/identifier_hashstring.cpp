#include "stdafx.h"

#include "core_identifier/HashString.h"


namespace RF::id {
///////////////////////////////////////////////////////////////////////////////

TEST( HashString, Basics )
{
	static constexpr HashString kTest = HashString( "Test" );
	static constexpr HashString kMirror = HashString( "Test" );
	static constexpr HashString kAlt = HashString( "Alt" );

	static_assert( kTest == kMirror );
	static_assert( kTest == "Test" );
	static_assert( kTest != kAlt );
	static_assert( kTest != "Alt" );
	static_assert( kTest != HashString{} );
	static_assert( HashString{} == HashString{} );

	static_assert( kTest.IsValid() );
	static_assert( kTest.GetHash() == RF_HASH_FROM_STRING_LITERAL( "Test" ) );
	static_assert( kTest.GetView() == "Test" );
	static_assert( HashString{}.IsValid() == false );
	static_assert( HashString{}.GetHash() == RF_HASH_FROM_STRING_LITERAL( "" ) );
	static_assert( HashString{}.GetView().data() == nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}

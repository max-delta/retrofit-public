#include "stdafx.h"

#include "core/idgen.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( IDGen, NonloopingIDGenerator )
{
	using Gen = NonloopingIDGenerator<uint8_t>;
	Gen gen;

	for( Gen::ID i = Gen::kInitial; i < Gen::kFinal; i++ )
	{
		Gen::ID const newID = gen.Generate();
		ASSERT_NE( newID, Gen::kInvalid );
		ASSERT_EQ( newID, i );
	}

	Gen::ID const lastID = gen.Generate();
	ASSERT_NE( lastID, Gen::kInvalid );
	ASSERT_EQ( lastID, Gen::kFinal );
}

///////////////////////////////////////////////////////////////////////////////
}

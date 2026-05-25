#include "stdafx.h"

#include "rftl/extension/string_hash.h"

#include "rftl/unordered_set"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( StringHash, TransparentHash )
{
	using Container = rftl::unordered_set<
		rftl::string,
		rftl::string_hash,
		rftl::equal_to<>>;
	Container const container = { "AAA" };

	ASSERT_TRUE( container.count( "AAA" ) );
	ASSERT_FALSE( container.count( "BBB" ) );
}



TEST( StringHash, PtrHash )
{
	// IMPORTANT: Small-string optimization makes it unsafe to use strings as
	//  the keys
	{
		rftl::string a = "AAA";
		void const* const aPtr = a.data();
		rftl::string b = rftl::move( a );
		void const* const bPtr = b.data();
		ASSERT_NE( aPtr, bPtr );
	}

	// Subtle, can't just use "AAA" directly as it will optimize to the same
	//  literal in multiple locations, so we instead put it on the stack
	char const storage1[] = "AAA";
	char const storage2[] = "AAA";

	using Container = rftl::unordered_set<
		rftl::string_view,
		rftl::string_ptr_only_hash_equals,
		rftl::string_ptr_only_hash_equals>;
	Container const container = { storage1 };
	rftl::string_view const& val = *container.begin();

	ASSERT_TRUE( container.count( storage1 ) );
	ASSERT_TRUE( container.count( val ) );
	ASSERT_TRUE( container.count( static_cast<rftl::string_view>( val ) ) );
	ASSERT_TRUE( container.count( static_cast<char const*>( val.data() ) ) );
	ASSERT_TRUE( container.count( static_cast<void const*>( val.data() ) ) );
	ASSERT_FALSE( container.count( storage2 ) );
}

///////////////////////////////////////////////////////////////////////////////
}

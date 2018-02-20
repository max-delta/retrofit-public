#include "stdafx.h"

#include "core/meta/MemberTest.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

RF_DECLARE_HAS_MEMBER_NAME_TEST(DesiredMemberName);



TEST( MemberTest, Basics )
{
	struct Has
	{
		int DesiredMemberName;
	};
	static_assert( RF_HAS_MEMBER_NAME( Has, DesiredMemberName ) == true, "Unexpected size" );

	struct HasNot
	{
		//
	};
	static_assert( RF_HAS_MEMBER_NAME( HasNot, DesiredMemberName ) == false, "Unexpected size" );
}

///////////////////////////////////////////////////////////////////////////////
}

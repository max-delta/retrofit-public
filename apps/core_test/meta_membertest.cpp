#include "stdafx.h"

#include "core/meta/MemberTest.h"


RF_DECLARE_HAS_PUBLIC_MEMBER_NAME_TEST( DesiredStaticMemberName );
RF_DECLARE_HAS_PUBLIC_MEMBER_NAME_TEST( DesiredNonStaticMemberName );

namespace RF {
///////////////////////////////////////////////////////////////////////////////

namespace details {

struct Has
{
	static int DesiredStaticMemberName;
	int DesiredNonStaticMemberName;
};

struct HasNot
{
	//
};

}



TEST( MemberTest, Basics )
{
	static_assert( RF_HAS_PUBLIC_MEMBER_NAME( details::Has, DesiredStaticMemberName ) == true, "Unexpected size" );
	static_assert( RF_HAS_PUBLIC_MEMBER_NAME( details::HasNot, DesiredStaticMemberName ) == false, "Unexpected size" );

	static_assert( RF_HAS_PUBLIC_MEMBER_NAME( details::Has, DesiredNonStaticMemberName ) == true, "Unexpected size" );
	static_assert( RF_HAS_PUBLIC_MEMBER_NAME( details::HasNot, DesiredNonStaticMemberName ) == false, "Unexpected size" );
}

///////////////////////////////////////////////////////////////////////////////
}

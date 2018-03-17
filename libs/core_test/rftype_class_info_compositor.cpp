#include "stdafx.h"

#include "core_rftype/CreateClassInfoDefinition.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

namespace details{

struct ClassWithStaticClassInfo
{
	RFTYPE_STATIC_CLASSINFO();

	char unusedPadding[16];
	double instance_d;
	static double static_d;
	void instance_v_call_v( void )
	{
		return;
	}
	static void static_v_call_v( void )
	{
		return;
	}
};
double ClassWithStaticClassInfo::static_d;

struct ClassWithoutStaticClassInfo
{
	char unused[16];
	double instance_d;
	static double static_d;
	void instance_v_call_v( void )
	{
		return;
	}
	static void static_v_call_v( void )
	{
		return;
	}
};
double ClassWithoutStaticClassInfo::static_d;

}

//
////
//////
////////
}} // Temp close of namespace, simulate .cpp file vs .h file
//////////

RFTYPE_DEFINE_STATIC_CLASSINFO( RF::reflect::details::ClassWithStaticClassInfo );
RFTYPE_CREATE_META( RF::reflect::details::ClassWithStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &ClassWithStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &ClassWithStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &ClassWithStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &ClassWithStaticClassInfo::instance_d );

	static_assert( std::is_same<RFTYPE_METATYPE(), ClassWithStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::ClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &ClassWithoutStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &ClassWithoutStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &ClassWithoutStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &ClassWithoutStaticClassInfo::instance_d );

	static_assert( std::is_same<RFTYPE_METATYPE(), ClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

//////////
namespace RF { namespace reflect { // Re-open namespace
////////
//////
////
//

TEST( RFType, Basics )
{
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::ClassWithStaticClassInfo>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &details::ClassWithStaticClassInfo::static_v_call_v );
	}
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::ClassWithoutStaticClassInfo>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &details::ClassWithoutStaticClassInfo::static_v_call_v );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

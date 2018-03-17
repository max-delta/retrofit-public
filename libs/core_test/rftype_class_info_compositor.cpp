#include "stdafx.h"

#include "core_rftype/CreateClassInfoDefinition.h"
#include "core_reflect/VirtualClass.h"


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

struct NonVirtualBaseClassWithoutStaticClassInfo
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
double NonVirtualBaseClassWithoutStaticClassInfo::static_d;

// NOTE: Private inheritance, cannot access base class via derived class
struct NonVirtualDerivedClassWithoutStaticClassInfo : private NonVirtualBaseClassWithoutStaticClassInfo
{
	char unused2[16];
};

struct VirtualBaseClassWithoutStaticClassInfo : public VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

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
double VirtualBaseClassWithoutStaticClassInfo::static_d;

struct VirtualDerivedClassWithoutStaticClassInfo : public VirtualBaseClassWithoutStaticClassInfo
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	char unused2[16];
};

struct MissingVirtualDerivedClassWithoutStaticClassInfo : public VirtualBaseClassWithoutStaticClassInfo
{
	// This is not present, so it will appear to be a base class instead
	//RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	char unused2[16];
};

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

RFTYPE_CREATE_META( RF::reflect::details::NonVirtualBaseClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &NonVirtualBaseClassWithoutStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &NonVirtualBaseClassWithoutStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &NonVirtualBaseClassWithoutStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &NonVirtualBaseClassWithoutStaticClassInfo::instance_d );

	static_assert( std::is_same<RFTYPE_METATYPE(), NonVirtualBaseClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::NonVirtualDerivedClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().BaseClass<NonVirtualBaseClassWithoutStaticClassInfo>();

	static_assert( std::is_same<RFTYPE_METATYPE(), NonVirtualDerivedClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::VirtualBaseClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &VirtualBaseClassWithoutStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &VirtualBaseClassWithoutStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &VirtualBaseClassWithoutStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &VirtualBaseClassWithoutStaticClassInfo::instance_d );

	static_assert( std::is_same<RFTYPE_METATYPE(), VirtualBaseClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::VirtualDerivedClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().BaseClass<VirtualBaseClassWithoutStaticClassInfo>();

	static_assert( std::is_same<RFTYPE_METATYPE(), VirtualDerivedClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::MissingVirtualDerivedClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().BaseClass<VirtualBaseClassWithoutStaticClassInfo>();

	static_assert( std::is_same<RFTYPE_METATYPE(), MissingVirtualDerivedClassWithoutStaticClassInfo>::value, "Unexpected type" );
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



TEST( RFType, NonVirtualChainInheritance )
{
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::NonVirtualBaseClassWithoutStaticClassInfo>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &details::NonVirtualBaseClassWithoutStaticClassInfo::static_v_call_v );
	}
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::NonVirtualDerivedClassWithoutStaticClassInfo>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 0 );
		ASSERT_TRUE( classInfo.mBaseTypes.size() == 1 );
		reflect::ClassInfo const& baseClassInfo = *( classInfo.mBaseTypes[0].mBaseClassInfo );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions[0].mAddress == &details::NonVirtualBaseClassWithoutStaticClassInfo::static_v_call_v );
	}
}



TEST( RFType, VirtualChainInheritance )
{
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::VirtualBaseClassWithoutStaticClassInfo>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &details::VirtualBaseClassWithoutStaticClassInfo::static_v_call_v );

		details::VirtualBaseClassWithoutStaticClassInfo targetClass;
		reflect::VirtualClass const* virtualClass = &targetClass;
		reflect::ClassInfo const& virtualClassInfo = *( virtualClass->GetVirtualClassInfo() );
		ASSERT_TRUE( &classInfo == &virtualClassInfo );
	}
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::VirtualDerivedClassWithoutStaticClassInfo>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 0 );
		ASSERT_TRUE( classInfo.mBaseTypes.size() == 1 );
		reflect::ClassInfo const& baseClassInfo = *( classInfo.mBaseTypes[0].mBaseClassInfo );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions[0].mAddress == &details::VirtualBaseClassWithoutStaticClassInfo::static_v_call_v );

		details::VirtualDerivedClassWithoutStaticClassInfo targetClass;
		reflect::VirtualClass const* virtualClass = &targetClass;
		reflect::ClassInfo const& virtualClassInfo = *( virtualClass->GetVirtualClassInfo() );
		ASSERT_TRUE( &classInfo == &virtualClassInfo );
	}
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::MissingVirtualDerivedClassWithoutStaticClassInfo>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 0 );
		ASSERT_TRUE( classInfo.mBaseTypes.size() == 1 );
		reflect::ClassInfo const& baseClassInfo = *( classInfo.mBaseTypes[0].mBaseClassInfo );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions[0].mAddress == &details::MissingVirtualDerivedClassWithoutStaticClassInfo::static_v_call_v );

		details::MissingVirtualDerivedClassWithoutStaticClassInfo targetClass;
		reflect::VirtualClass const* virtualClass = &targetClass;
		reflect::ClassInfo const& virtualClassInfo = *( virtualClass->GetVirtualClassInfo() );
		ASSERT_TRUE( &classInfo != &virtualClassInfo );
		ASSERT_TRUE( &baseClassInfo == &virtualClassInfo );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

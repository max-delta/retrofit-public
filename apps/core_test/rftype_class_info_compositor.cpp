#include "stdafx.h"

#include "core_rftype/CreateClassInfoDefinition.h"
#include "core_reflect/VirtualClass.h"


RF_CLANG_IGNORE( "-Winvalid-offsetof" );

namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////

namespace details {

struct ClassWithStaticClassInfo
{
	RFTYPE_STATIC_CLASSINFO();

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

struct NonVirtualDerivedClassWithoutStaticClassInfo : public NonVirtualBaseClassWithoutStaticClassInfo
{
	char unused2[16];
	float instance_f;
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
	float instance_f;
};

struct MissingVirtualDerivedClassWithoutStaticClassInfo : public VirtualBaseClassWithoutStaticClassInfo
{
	// This is not present, so it will appear to be a base class instead
	//RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	char unused2[16];
	float instance_f;
};

namespace diamond {

RF_ACK_UNSAFE_INHERITANCE
struct DiamondTop : virtual public VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	char unusedTop[16];
};
RF_ACK_UNSAFE_INHERITANCE
struct DiamondLeft : virtual public DiamondTop
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	char unusedLeft[16];
};
RF_ACK_UNSAFE_INHERITANCE
struct DiamondRight : virtual public DiamondTop
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	char unusedRight[16];
};
RF_ACK_UNSAFE_INHERITANCE
struct DiamondBottom : virtual public DiamondLeft, virtual public DiamondRight
{
	// If not present, the absence of the virtual loolup should make this class
	//  ambigious and fail to compile
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	char unusedBottom[16];
};

}
namespace nested {

struct Contents
{
	char unused[16];
	float instance_f;
};
struct Containing
{
	char unused[16];
	Contents contents;
};

}
}

//
////
//////
////////
} // Temp close of namespace, simulate .cpp file vs .h file
//////////

RFTYPE_DEFINE_STATIC_CLASSINFO( RF::reflect::details::ClassWithStaticClassInfo );
RFTYPE_CREATE_META( RF::reflect::details::ClassWithStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &ClassWithStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &ClassWithStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &ClassWithStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &ClassWithStaticClassInfo::instance_d );

	static_assert( rftl::is_same<RFTYPE_METATYPE(), ClassWithStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::ClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &ClassWithoutStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &ClassWithoutStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &ClassWithoutStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &ClassWithoutStaticClassInfo::instance_d );

	static_assert( rftl::is_same<RFTYPE_METATYPE(), ClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::NonVirtualBaseClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &NonVirtualBaseClassWithoutStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &NonVirtualBaseClassWithoutStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &NonVirtualBaseClassWithoutStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &NonVirtualBaseClassWithoutStaticClassInfo::instance_d );

	static_assert( rftl::is_same<RFTYPE_METATYPE(), NonVirtualBaseClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::NonVirtualDerivedClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().BaseClass<NonVirtualBaseClassWithoutStaticClassInfo>();
	RFTYPE_META().RawProperty( "instance_f", &NonVirtualDerivedClassWithoutStaticClassInfo::instance_f );

	static_assert( rftl::is_same<RFTYPE_METATYPE(), NonVirtualDerivedClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::VirtualBaseClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().Method( "static_v_call_v", &VirtualBaseClassWithoutStaticClassInfo::static_v_call_v );
	RFTYPE_META().Method( "instance_v_call_v", &VirtualBaseClassWithoutStaticClassInfo::instance_v_call_v );
	RFTYPE_META().RawProperty( "static_d", &VirtualBaseClassWithoutStaticClassInfo::static_d );
	RFTYPE_META().RawProperty( "instance_d", &VirtualBaseClassWithoutStaticClassInfo::instance_d );

	static_assert( rftl::is_same<RFTYPE_METATYPE(), VirtualBaseClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::VirtualDerivedClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().BaseClass<VirtualBaseClassWithoutStaticClassInfo>();
	RFTYPE_META().RawProperty( "instance_f", &VirtualDerivedClassWithoutStaticClassInfo::instance_f );

	static_assert( rftl::is_same<RFTYPE_METATYPE(), VirtualDerivedClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::MissingVirtualDerivedClassWithoutStaticClassInfo )
{
	using namespace ::RF::reflect::details;
	RFTYPE_META().BaseClass<VirtualBaseClassWithoutStaticClassInfo>();
	RFTYPE_META().RawProperty( "instance_f", &MissingVirtualDerivedClassWithoutStaticClassInfo::instance_f );

	static_assert( rftl::is_same<RFTYPE_METATYPE(), MissingVirtualDerivedClassWithoutStaticClassInfo>::value, "Unexpected type" );
}

RFTYPE_CREATE_META( RF::reflect::details::diamond::DiamondTop )
{
}
RFTYPE_CREATE_META( RF::reflect::details::diamond::DiamondLeft )
{
	RFTYPE_META().ComplexBaseClass<RF::reflect::details::diamond::DiamondTop>();
}
RFTYPE_CREATE_META( RF::reflect::details::diamond::DiamondRight )
{
	RFTYPE_META().ComplexBaseClass<RF::reflect::details::diamond::DiamondTop>();
}
RFTYPE_CREATE_META( RF::reflect::details::diamond::DiamondBottom )
{
	RFTYPE_META().ComplexBaseClass<RF::reflect::details::diamond::DiamondLeft>();
	RFTYPE_META().ComplexBaseClass<RF::reflect::details::diamond::DiamondRight>();
}

RFTYPE_CREATE_META( RF::reflect::details::nested::Contents )
{
	using namespace ::RF::reflect::details::nested;
	RFTYPE_META().RawProperty( "instance_f", &Contents::instance_f );
}
RFTYPE_CREATE_META( RF::reflect::details::nested::Containing )
{
	using namespace ::RF::reflect::details::nested;
	RFTYPE_META().RawProperty( "contents", &Containing::contents );
}

//////////
namespace RF::reflect { // Re-open namespace
////////
//////
////
//



TEST( RFType, AutoInit )
{
	// ClassInfo starts uninitialized, and needs to have the builder run on it
	//  to pick up a bunch of the basics
	{
		reflect::ClassInfo stub = reflect::ClassInfo( ExplicitDefaultConstruct{} );
		ASSERT_TRUE( stub.mIsDefaultConstructible == false );

		struct Stub
		{
		};
		reflect::builder::CreateClassInfo<Stub>( stub );
		ASSERT_TRUE( stub.mIsDefaultConstructible == true );
	}

	// The compositor should make sure the builder gets run on the ClassInfos
	//  as part of the overall composition process
	{
		using TargetClass = details::ClassWithStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();
		ASSERT_TRUE( classInfo.mIsDestructible );
	}
	{
		using TargetClass = details::ClassWithoutStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();
		ASSERT_TRUE( classInfo.mIsDestructible );
	}
}



TEST( RFType, Basics )
{
	{
		using TargetClass = details::ClassWithStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &TargetClass::static_v_call_v );
		ASSERT_STREQ( classInfo.mStaticFunctions[0].mIdentifier, "static_v_call_v" );

		ASSERT_EQ( classInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( classInfo.mNonStaticVariables[0].mOffset, offsetof( TargetClass, instance_d ) );
		ASSERT_STREQ( classInfo.mNonStaticVariables[0].mIdentifier, "instance_d" );
	}
	{
		using TargetClass = details::ClassWithoutStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();
		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &TargetClass::static_v_call_v );
		ASSERT_STREQ( classInfo.mStaticFunctions[0].mIdentifier, "static_v_call_v" );

		ASSERT_EQ( classInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( classInfo.mNonStaticVariables[0].mOffset, offsetof( TargetClass, instance_d ) );
		ASSERT_STREQ( classInfo.mNonStaticVariables[0].mIdentifier, "instance_d" );
	}
}



TEST( RFType, NonVirtualChainInheritance )
{
	{
		using TargetClass = details::NonVirtualBaseClassWithoutStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();

		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, false );
		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClass, false );

		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &TargetClass::static_v_call_v );
		ASSERT_STREQ( classInfo.mStaticFunctions[0].mIdentifier, "static_v_call_v" );

		ASSERT_EQ( classInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( classInfo.mNonStaticVariables[0].mOffset, offsetof( TargetClass, instance_d ) );
		ASSERT_STREQ( classInfo.mNonStaticVariables[0].mIdentifier, "instance_d" );
	}
	{
		using TargetClass = details::NonVirtualDerivedClassWithoutStaticClassInfo;
		using BaseClass = details::NonVirtualBaseClassWithoutStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();

		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, false );
		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClass, false );

		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 0 );
		ASSERT_TRUE( classInfo.mBaseTypes.size() == 1 );

		reflect::ClassInfo const& baseClassInfo = *( classInfo.mBaseTypes[0].mBaseClassInfo );

		ASSERT_EQ( baseClassInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, false );
		ASSERT_EQ( baseClassInfo.mVirtualRootInfo.mDerivesFromVirtualClass, false );

		ASSERT_TRUE( baseClassInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions[0].mAddress == &BaseClass::static_v_call_v );
		ASSERT_STREQ( baseClassInfo.mStaticFunctions[0].mIdentifier, "static_v_call_v" );

		ASSERT_EQ( classInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( classInfo.mNonStaticVariables[0].mOffset, offsetof( TargetClass, instance_f ) );
		ASSERT_STREQ( classInfo.mNonStaticVariables[0].mIdentifier, "instance_f" );
		ASSERT_EQ( baseClassInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( baseClassInfo.mNonStaticVariables[0].mOffset, offsetof( BaseClass, instance_d ) );
		ASSERT_STREQ( baseClassInfo.mNonStaticVariables[0].mIdentifier, "instance_d" );
	}
}



TEST( RFType, VirtualChainInheritance )
{
	{
		using TargetClass = details::VirtualBaseClassWithoutStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();

		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, true );
		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClass, true );
		// TODO: These should be null once the machinery for determining
		//  they're trivial at compile-time is available
		ASSERT_NE( classInfo.mVirtualRootInfo.mGetRootNonDestructingPointerFromCurrent, nullptr );
		ASSERT_NE( classInfo.mVirtualRootInfo.mGetRootPointerFromCurrent, nullptr );

		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( classInfo.mStaticFunctions[0].mAddress == &TargetClass::static_v_call_v );
		ASSERT_STREQ( classInfo.mStaticFunctions[0].mIdentifier, "static_v_call_v" );

		TargetClass targetClass;
		reflect::VirtualClass const* virtualClass = &targetClass;
		reflect::ClassInfo const& virtualClassInfo = *( virtualClass->GetVirtualClassInfo() );
		ASSERT_TRUE( &classInfo == &virtualClassInfo );

		ASSERT_EQ( classInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( classInfo.mNonStaticVariables[0].mOffset, offsetof( TargetClass, instance_d ) );
		ASSERT_STREQ( classInfo.mNonStaticVariables[0].mIdentifier, "instance_d" );
	}
	{
		using TargetClass = details::VirtualDerivedClassWithoutStaticClassInfo;
		using BaseClass = details::VirtualBaseClassWithoutStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();

		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, true );
		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClass, true );
		// TODO: These should be null once the machinery for determining
		//  they're trivial at compile-time is available
		ASSERT_NE( classInfo.mVirtualRootInfo.mGetRootNonDestructingPointerFromCurrent, nullptr );
		ASSERT_NE( classInfo.mVirtualRootInfo.mGetRootPointerFromCurrent, nullptr );

		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 0 );
		ASSERT_TRUE( classInfo.mBaseTypes.size() == 1 );

		reflect::ClassInfo const& baseClassInfo = *( classInfo.mBaseTypes[0].mBaseClassInfo );

		ASSERT_EQ( baseClassInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, true );
		ASSERT_EQ( baseClassInfo.mVirtualRootInfo.mDerivesFromVirtualClass, true );
		// TODO: These should be null once the machinery for determining
		//  they're trivial at compile-time is available
		ASSERT_NE( baseClassInfo.mVirtualRootInfo.mGetRootNonDestructingPointerFromCurrent, nullptr );
		ASSERT_NE( baseClassInfo.mVirtualRootInfo.mGetRootPointerFromCurrent, nullptr );

		ASSERT_TRUE( baseClassInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions[0].mAddress == &BaseClass::static_v_call_v );
		ASSERT_STREQ( baseClassInfo.mStaticFunctions[0].mIdentifier, "static_v_call_v" );

		TargetClass targetClass;
		reflect::VirtualClass const* virtualClass = &targetClass;
		reflect::ClassInfo const& virtualClassInfo = *( virtualClass->GetVirtualClassInfo() );
		ASSERT_TRUE( &classInfo == &virtualClassInfo );

		ASSERT_EQ( classInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( classInfo.mNonStaticVariables[0].mOffset, offsetof( TargetClass, instance_f ) );
		ASSERT_STREQ( classInfo.mNonStaticVariables[0].mIdentifier, "instance_f" );
		ASSERT_EQ( baseClassInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( baseClassInfo.mNonStaticVariables[0].mOffset, offsetof( BaseClass, instance_d ) );
		ASSERT_STREQ( baseClassInfo.mNonStaticVariables[0].mIdentifier, "instance_d" );
	}
	{
		using TargetClass = details::MissingVirtualDerivedClassWithoutStaticClassInfo;
		using BaseClass = details::VirtualBaseClassWithoutStaticClassInfo;

		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<TargetClass>();

		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, true );
		ASSERT_EQ( classInfo.mVirtualRootInfo.mDerivesFromVirtualClass, true );
		// TODO: These should be null once the machinery for determining
		//  they're trivial at compile-time is available
		ASSERT_NE( classInfo.mVirtualRootInfo.mGetRootNonDestructingPointerFromCurrent, nullptr );
		ASSERT_NE( classInfo.mVirtualRootInfo.mGetRootPointerFromCurrent, nullptr );

		ASSERT_TRUE( classInfo.mStaticFunctions.size() == 0 );
		ASSERT_TRUE( classInfo.mBaseTypes.size() == 1 );

		reflect::ClassInfo const& baseClassInfo = *( classInfo.mBaseTypes[0].mBaseClassInfo );

		ASSERT_EQ( baseClassInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor, true );
		ASSERT_EQ( baseClassInfo.mVirtualRootInfo.mDerivesFromVirtualClass, true );
		// TODO: These should be null once the machinery for determining
		//  they're trivial at compile-time is available
		ASSERT_NE( baseClassInfo.mVirtualRootInfo.mGetRootNonDestructingPointerFromCurrent, nullptr );
		ASSERT_NE( baseClassInfo.mVirtualRootInfo.mGetRootPointerFromCurrent, nullptr );

		ASSERT_TRUE( baseClassInfo.mStaticFunctions.size() == 1 );
		ASSERT_TRUE( baseClassInfo.mStaticFunctions[0].mAddress == &TargetClass::static_v_call_v );
		ASSERT_STREQ( baseClassInfo.mStaticFunctions[0].mIdentifier, "static_v_call_v" );

		TargetClass targetClass;
		reflect::VirtualClass const* virtualClass = &targetClass;
		reflect::ClassInfo const& virtualClassInfo = *( virtualClass->GetVirtualClassInfo() );
		ASSERT_TRUE( &classInfo != &virtualClassInfo );
		ASSERT_TRUE( &baseClassInfo == &virtualClassInfo );

		ASSERT_EQ( classInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( classInfo.mNonStaticVariables[0].mOffset, offsetof( TargetClass, instance_f ) );
		ASSERT_STREQ( classInfo.mNonStaticVariables[0].mIdentifier, "instance_f" );
		ASSERT_EQ( baseClassInfo.mNonStaticVariables.size(), 1 );
		ASSERT_EQ( baseClassInfo.mNonStaticVariables[0].mOffset, offsetof( BaseClass, instance_d ) );
		ASSERT_STREQ( baseClassInfo.mNonStaticVariables[0].mIdentifier, "instance_d" );
	}
}



TEST( RFType, DiamondInheritance )
{
	using namespace RF::reflect::details::diamond;

	DiamondTop topInstance;
	DiamondLeft leftInstance;
	DiamondRight rightInstance;
	DiamondBottom bottomInstance;

	VirtualClass const* topPtr = &topInstance;
	VirtualClass const* leftPtr = &leftInstance;
	VirtualClass const* rightPtr = &rightInstance;
	VirtualClass const* bottomPtr = &bottomInstance;

	ClassInfo const* topCI = topPtr->GetVirtualClassInfo();
	ClassInfo const* leftCI = leftPtr->GetVirtualClassInfo();
	ClassInfo const* rightCI = rightPtr->GetVirtualClassInfo();
	ClassInfo const* bottomCI = bottomPtr->GetVirtualClassInfo();

	// These PROBABLY won't be COMDAT-folded, the complexity to do so would be
	//  quite high given the machinery at play
	ASSERT_NE( topCI, leftCI );
	ASSERT_NE( topCI, rightCI );
	ASSERT_NE( topCI, bottomCI );
	ASSERT_NE( leftCI, rightCI );
	ASSERT_NE( leftCI, bottomCI );
	ASSERT_NE( rightCI, bottomCI );
}



TEST( RFType, Nested )
{
	{
		reflect::ClassInfo const& classInfo = rftype::GetClassInfo<details::nested::Containing>();
		ASSERT_TRUE( classInfo.mNonStaticVariables.size() == 1 );
		MemberVariableInfo const& varInfo = classInfo.mNonStaticVariables[0];
		ASSERT_STREQ( varInfo.mIdentifier, "contents" );
		ASSERT_TRUE( varInfo.mMutable == true );
		ASSERT_TRUE( varInfo.mOffset == offsetof( details::nested::Containing, contents ) );
		ASSERT_TRUE( varInfo.mSize == sizeof( details::nested::Contents ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::Invalid );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == &( rftype::GetClassInfo<details::nested::Contents>() ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

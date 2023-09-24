#include "stdafx.h"

#include "RFType/CreateClassInfoDefinition.h"
#include "RFType/GlobalTypeDatabase.h"

#include "core_rftype/ClassInfoAccessor.h"

#include "RFType/Example.h"



namespace RF::rftype::test {
///////////////////////////////////////////////////////////////////////////////
struct RFTypeConstructionTest
{
	int mInt = 7;
};
///////////////////////////////////////////////////////////////////////////////
}

RFTYPE_CREATE_META( RF::rftype::test::RFTypeConstructionTest )
{
	using RF::rftype::test::RFTypeConstructionTest;
	RFTYPE_META().RawProperty( "mInt", &RFTypeConstructionTest::mInt );
	RFTYPE_REGISTER_DEFAULT_CREATOR();
}

namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

TEST( RFType, CrossDllAccess )
{
	reflect::ClassInfo const& classInfoViaStatic = GetClassInfo<rftype_example::ExampleWithStaticClassInfo>();
	reflect::ClassInfo const& classInfoViaExport = GetClassInfo<rftype_example::ExampleWithoutStaticClassInfo>();

	TypeDatabase const& typeDatabase = GetGlobalTypeDatabase();
	reflect::ClassInfo const* classInfoStaticViaLookup = typeDatabase.GetClassInfoByName( "ExampleWithStaticClassInfo" );
	ASSERT_TRUE( classInfoStaticViaLookup != nullptr );
	reflect::ClassInfo const* classInfoExportViaLookup = typeDatabase.GetClassInfoByName( "ExampleWithoutStaticClassInfo" );
	ASSERT_TRUE( classInfoExportViaLookup != nullptr );

	ASSERT_EQ( classInfoStaticViaLookup, &classInfoViaStatic );
	ASSERT_EQ( classInfoExportViaLookup, &classInfoViaExport );
}



TEST( RFType, CrossDllExtension )
{
	using namespace reflect;

	rftype_example::ExampleWithExtensionAsMember object;
	object.mExampleExtensionAsMember.push_back( 5 );
	object.mExampleExtensionAsMember.push_back( 7 );

	TypeDatabase const& typeDatabase = GetGlobalTypeDatabase();
	ClassInfo const* classInfo = typeDatabase.GetClassInfoByName( "ExampleWithExtensionAsMember" );
	ASSERT_NE( classInfo, nullptr );

	ASSERT_EQ( classInfo->mNonStaticVariables.size(), 1 );
	MemberVariableInfo const& memVarInfo = classInfo->mNonStaticVariables[0];
	void* const varLoc = reinterpret_cast<uint8_t*>( &object ) + memVarInfo.mOffset;
	ASSERT_NE( varLoc, nullptr );
	ExtensionAccessor const* const accessor = memVarInfo.mVariableTypeInfo.mAccessor;
	ASSERT_NE( accessor, nullptr );

	if( accessor->mBeginAccess != nullptr )
	{
		accessor->mBeginAccess( varLoc );
	}

	ASSERT_NE( accessor->mGetNumVariables, nullptr );
	size_t const numVars = accessor->mGetNumVariables( varLoc );
	ASSERT_EQ( numVars, 2 );

	ASSERT_NE( accessor->mGetKeyInfoByIndex, nullptr );
	VariableTypeInfo const i0k_info = accessor->mGetKeyInfoByIndex( varLoc, 0 );
	ASSERT_EQ( i0k_info.mValueType, Value::DetermineType<size_t>() );
	VariableTypeInfo const i1k_info = accessor->mGetKeyInfoByIndex( varLoc, 1 );
	ASSERT_EQ( i1k_info.mValueType, Value::DetermineType<size_t>() );

	size_t const i0k = 0;
	size_t const i1k = 1;
	VariableTypeInfo readKeyInfo = {};
	readKeyInfo.mValueType = Value::DetermineType<size_t>();

	ASSERT_NE( accessor->mGetTargetInfoByKey, nullptr );
	VariableTypeInfo const i0t_info = accessor->mGetTargetInfoByKey( varLoc, &i0k, readKeyInfo );
	ASSERT_EQ( i0t_info.mValueType, Value::Type::UInt16 );
	VariableTypeInfo const i1t_info = accessor->mGetTargetInfoByKey( varLoc, &i1k, readKeyInfo );
	ASSERT_EQ( i1t_info.mValueType, Value::Type::UInt16 );

	void const* i0v = nullptr;
	void const* i1v = nullptr;
	VariableTypeInfo i0vInfo = {};
	VariableTypeInfo i1vInfo = {};
	ASSERT_NE( accessor->mGetTargetByKey, nullptr );
	bool const i0Read = accessor->mGetTargetByKey( varLoc, &i0k, readKeyInfo, i0v, i0vInfo );
	ASSERT_TRUE( i0Read );
	ASSERT_EQ( i0t_info.mValueType, i0t_info.mValueType );
	bool const i1Read = accessor->mGetTargetByKey( varLoc, &i1k, readKeyInfo, i1v, i1vInfo );
	ASSERT_TRUE( i1Read );
	ASSERT_EQ( i1t_info.mValueType, i1t_info.mValueType );

	uint16_t const* const i0p = reinterpret_cast<uint16_t const*>( i0v );
	ASSERT_NE( i0p, nullptr );
	ASSERT_EQ( *i0p, 5 );
	uint16_t const* const i1p = reinterpret_cast<uint16_t const*>( i1v );
	ASSERT_NE( i1p, nullptr );
	ASSERT_EQ( *i1p, 7 );

	if( accessor->mEndAccess != nullptr )
	{
		accessor->mEndAccess( varLoc );
	}

	if( accessor->mBeginMutation != nullptr )
	{
		accessor->mBeginMutation( varLoc );
	}

	size_t const i5k = 5;
	VariableTypeInfo writeKeyInfo = {};
	writeKeyInfo.mValueType = Value::DetermineType<size_t>();
	uint16_t const i5v = 43;
	VariableTypeInfo writeValueInfo = {};
	writeValueInfo.mValueType = Value::DetermineType<uint16_t>();

	ASSERT_NE( accessor->mInsertVariableViaCopy, nullptr );
	bool const i5Write = accessor->mInsertVariableViaCopy( varLoc, &i5k, writeKeyInfo, &i5v, writeValueInfo );
	ASSERT_TRUE( i5Write );

	if( accessor->mEndMutation != nullptr )
	{
		accessor->mEndMutation( varLoc );
	}

	ASSERT_EQ( object.mExampleExtensionAsMember.size(), 6 );
	ASSERT_EQ( object.mExampleExtensionAsMember.at( 0 ), 5 );
	ASSERT_EQ( object.mExampleExtensionAsMember.at( 1 ), 7 );
	ASSERT_EQ( object.mExampleExtensionAsMember.at( 5 ), 43 );
}



TEST( RFType, Construction )
{
	TypeDatabase const& typeDatabase = GetGlobalTypeDatabase();

	reflect::ClassInfo const& classInfo = GetClassInfo<test::RFTypeConstructionTest>();
	ConstructedType constructed = typeDatabase.ConstructClass( classInfo );
	ASSERT_EQ( constructed.mClassInfo, &classInfo );
	ASSERT_NE( constructed.mLocation, nullptr );

	test::RFTypeConstructionTest const* const casted =
		reinterpret_cast<test::RFTypeConstructionTest*>(
			constructed.mLocation.Get() );
	ASSERT_EQ( casted->mInt, 7 );
}

///////////////////////////////////////////////////////////////////////////////
}

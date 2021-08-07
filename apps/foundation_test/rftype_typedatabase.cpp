#include "stdafx.h"

#include "RFType/TypeDatabase.h"
#include "core_rftype/ClassInfoAccessor.h"

#include "RFType/Example.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

TEST( RFType, CrossDllAccess )
{
	reflect::ClassInfo const& classInfoViaStatic = GetClassInfo<rftype_example::ExampleWithStaticClassInfo>();
	reflect::ClassInfo const& classInfoViaExport = GetClassInfo<rftype_example::ExampleWithoutStaticClassInfo>();

	TypeDatabase const& typeDatabase = TypeDatabase::GetGlobalInstance();
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

	TypeDatabase const& typeDatabase = TypeDatabase::GetGlobalInstance();
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

	ASSERT_NE( accessor->mGetVariableKeyInfoByIndex, nullptr );
	VariableTypeInfo const i0k_info = accessor->mGetVariableKeyInfoByIndex( varLoc, 0 );
	ASSERT_EQ( i0k_info.mValueType, Value::DetermineType<size_t>() );
	VariableTypeInfo const i1k_info = accessor->mGetVariableKeyInfoByIndex( varLoc, 1 );
	ASSERT_EQ( i1k_info.mValueType, Value::DetermineType<size_t>() );

	size_t const i0k = 0;
	size_t const i1k = 1;
	VariableTypeInfo readKeyInfo = {};
	readKeyInfo.mValueType = Value::DetermineType<size_t>();

	ASSERT_NE( accessor->mGetVariableTargetInfoByKey, nullptr );
	VariableTypeInfo const i0t_info = accessor->mGetVariableTargetInfoByKey( varLoc, &i0k, readKeyInfo );
	ASSERT_EQ( i0t_info.mValueType, Value::Type::UInt16 );
	VariableTypeInfo const i1t_info = accessor->mGetVariableTargetInfoByKey( varLoc, &i1k, readKeyInfo );
	ASSERT_EQ( i1t_info.mValueType, Value::Type::UInt16 );

	void const* i0v = nullptr;
	void const* i1v = nullptr;
	VariableTypeInfo i0vInfo = {};
	VariableTypeInfo i1vInfo = {};
	ASSERT_NE( accessor->mGetVariableTargetByKey, nullptr );
	bool const i0Read = accessor->mGetVariableTargetByKey( varLoc, &i0k, readKeyInfo, i0v, i0vInfo );
	ASSERT_TRUE( i0Read );
	ASSERT_EQ( i0t_info.mValueType, i0t_info.mValueType );
	bool const i1Read = accessor->mGetVariableTargetByKey( varLoc, &i1k, readKeyInfo, i1v, i1vInfo );
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

///////////////////////////////////////////////////////////////////////////////
}

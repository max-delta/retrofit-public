#include "stdafx.h"

#include "RFType/TypeDatabase.h"
#include "core_rftype/ClassInfoAccessor.h"

#include "RFType/Example.h"


namespace RF { namespace rftype {
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

	Value const i0k = Value( static_cast<size_t>( 0 ) );
	Value const i1k = Value( static_cast<size_t>( 1 ) );

	ASSERT_NE( accessor->mGetVariableTargetInfoByValue, nullptr );
	VariableTypeInfo const i0t_info = accessor->mGetVariableTargetInfoByValue( varLoc, i0k );
	ASSERT_EQ( i0t_info.mValueType, Value::Type::UInt16 );
	VariableTypeInfo const i1t_info = accessor->mGetVariableTargetInfoByValue( varLoc, i1k );
	ASSERT_EQ( i1t_info.mValueType, Value::Type::UInt16 );

	ASSERT_NE( accessor->mGetVariableTargetAsValueByKeyAsValue, nullptr );
	Value const i0v = accessor->mGetVariableTargetAsValueByKeyAsValue( varLoc, i0k );
	ASSERT_EQ( i0v.GetStoredType(), i0t_info.mValueType );
	Value const i1v = accessor->mGetVariableTargetAsValueByKeyAsValue( varLoc, i1k );
	ASSERT_EQ( i1v.GetStoredType(), i1t_info.mValueType );

	uint16_t const* const i0p = i0v.GetAs<uint16_t>();
	ASSERT_NE( i0p, nullptr );
	ASSERT_EQ( *i0p, 5 );
	uint16_t const* const i1p = i1v.GetAs<uint16_t>();
	ASSERT_NE( i1p, nullptr );
	ASSERT_EQ( *i1p, 7 );

	if( accessor->mEndAccess != nullptr )
	{
		accessor->mEndAccess( varLoc );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

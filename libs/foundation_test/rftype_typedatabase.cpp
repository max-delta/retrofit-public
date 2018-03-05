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

///////////////////////////////////////////////////////////////////////////////
}}

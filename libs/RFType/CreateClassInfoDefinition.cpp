#include "stdafx.h"
#include "CreateClassInfoDefinition.h"

#include "RFType/TypeDatabase.h"

#include "core_rftype/Identifier.h"

#include "rftl/cstdlib"
#include "rftl/string"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

void GlobalRegisterNewClassByName( char const* name, reflect::ClassInfo const& classInfo )
{
	bool const success = TypeDatabase::GetGlobalMutableInstance().RegisterNewClassByName( name, classInfo );
	RF_ASSERT( success );
	if( success == false )
	{
		rftl::abort();
	}
}



void GlobalRegisterNewClassByQualifiedName( char const* name, reflect::ClassInfo const& classInfo )
{
	rftl::string buf = name;
	SanitizeIdentifier( name, buf.data() );
	bool const success = TypeDatabase::GetGlobalMutableInstance().RegisterNewClassByName( buf.c_str(), classInfo );
	RF_ASSERT( success );
	if( success == false )
	{
		rftl::abort();
	}
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "CreateClassInfoDefinition.h"

#include "RFType/TypeDatabase.h"

#include "rftl/cstdlib"


namespace RF { namespace rftype {
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

///////////////////////////////////////////////////////////////////////////////
}}

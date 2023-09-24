#include "stdafx.h"
#include "CreateClassInfoDefinition.h"

#include "RFType/GlobalTypeDatabase.h"

#include "core_rftype/Identifier.h"

#include "rftl/cstdlib"
#include "rftl/string"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

void GlobalRegisterNewClassByName( char const* name, reflect::ClassInfo const& classInfo )
{
	bool const success = GetGlobalMutableTypeDatabase().RegisterNewClassByName( name, classInfo );
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
	bool const success = GetGlobalMutableTypeDatabase().RegisterNewClassByName( buf.c_str(), classInfo );
	RF_ASSERT( success );
	if( success == false )
	{
		rftl::abort();
	}
}



void GlobalRegisterNewConstructorForClass( rftl::function<ConstructedType()>&& constructor, reflect::ClassInfo const& classInfo )
{
	bool const success = GetGlobalMutableTypeDatabase().RegisterNewConstructorForClass( rftl::move( constructor ), classInfo );
	RF_ASSERT( success );
	if( success == false )
	{
		rftl::abort();
	}
}

///////////////////////////////////////////////////////////////////////////////
}

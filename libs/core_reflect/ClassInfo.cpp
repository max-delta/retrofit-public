#include "stdafx.h"
#include "ClassInfo.h"

namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

char const* ClassInfo::StoreString( char const* string )
{
	rftl::string const& storedString = mIdentifierStorage.emplace_back( string );
	return storedString.data();
}

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "GlobalTypeDatabase.h"

#include "core/meta/LazyInitSingleton.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static TypeDatabase& GetOrCreateGlobalTypeDatabase()
{
	return GetOrInitFunctionStaticScopedSingleton<TypeDatabase>();
}

}
///////////////////////////////////////////////////////////////////////////////

TypeDatabase& GetGlobalMutableTypeDatabase()
{
	return details::GetOrCreateGlobalTypeDatabase();
}



TypeDatabase const& GetGlobalTypeDatabase()
{
	return details::GetOrCreateGlobalTypeDatabase();
}

///////////////////////////////////////////////////////////////////////////////
}

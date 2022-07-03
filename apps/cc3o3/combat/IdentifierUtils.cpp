#include "stdafx.h"
#include "IdentifierUtils.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////
namespace details {

using EntityClassEntry = rftl::pair<EntityClass, char const*>;
static constexpr EntityClassEntry kEntityClassEntries[] = {
	{ EntityClass::Peasant, "Peasant" },
	{ EntityClass::Hero, "Hero" },
	{ EntityClass::Monster, "Monster" },
};

}
///////////////////////////////////////////////////////////////////////////////

EntityClass ReadEntityClassFromString( rftl::string_view str )
{
	for( details::EntityClassEntry const& entry : details::kEntityClassEntries )
	{
		if( str == entry.second )
		{
			return entry.first;
		}
	}

	return EntityClass::Invalid;
}

///////////////////////////////////////////////////////////////////////////////
}

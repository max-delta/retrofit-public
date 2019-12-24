#include "stdafx.h"
#include "ComponentResolver.h"

#include "core_component/ComponentRef.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

bool ComponentResolver::operator()( component::ComponentRef const& from, component::ResolvedComponentType to ) const
{
	return from.GetComponentType() == to;
}

///////////////////////////////////////////////////////////////////////////////
}

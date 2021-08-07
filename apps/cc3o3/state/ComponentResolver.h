#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

struct ComponentResolver
{
	template<typename T>
	component::ResolvedComponentType operator()() const;

	bool operator()( component::ComponentRef const& from, component::ResolvedComponentType to ) const;
};

///////////////////////////////////////////////////////////////////////////////
}

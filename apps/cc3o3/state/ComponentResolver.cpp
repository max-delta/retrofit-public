#include "stdafx.h"
#include "ComponentResolver.h"

#include "core_component/ComponentRef.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

#define RF_COMP_PAIR( ID, TYPE ) \
	template<> \
	component::ResolvedComponentType state::ComponentResolver::operator()<TYPE>() const \
	{ \
		return ID; \
	}

// All components listed and paired here
namespace comp {
class Meta;
}
namespace details::id {
enum : ComponentID
{
	Invalid = component::kInvalidResolvedComponentType,
	Meta,
};
}
RF_COMP_PAIR( details::id::Meta, comp::Meta );

#undef RF_COMP_PAIR

///////////////////////////////////////////////////////////////////////////////

bool ComponentResolver::operator()( component::ComponentRef const& from, component::ResolvedComponentType to ) const
{
	return from.GetComponentType() == to;
}

///////////////////////////////////////////////////////////////////////////////
}

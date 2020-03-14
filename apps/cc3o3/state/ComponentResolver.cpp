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
class Roster;
class Progression;
class OverworldVisual;
class OverworldMovement;
class UINavigation;
}
namespace details::id {
enum : ComponentID
{
	Invalid = component::kInvalidResolvedComponentType,
	Meta,
	Roster,
	Progression,
	OverworldVisual,
	OverworldMovement,
	UINavigation,
};
}
RF_COMP_PAIR( details::id::Meta, comp::Meta );
RF_COMP_PAIR( details::id::Roster, comp::Roster );
RF_COMP_PAIR( details::id::Progression, comp::Progression );
RF_COMP_PAIR( details::id::OverworldVisual, comp::OverworldVisual );
RF_COMP_PAIR( details::id::OverworldMovement, comp::OverworldMovement );
RF_COMP_PAIR( details::id::UINavigation, comp::UINavigation );

#undef RF_COMP_PAIR

///////////////////////////////////////////////////////////////////////////////

bool ComponentResolver::operator()( component::ComponentRef const& from, component::ResolvedComponentType to ) const
{
	return from.GetComponentType() == to;
}

///////////////////////////////////////////////////////////////////////////////
}

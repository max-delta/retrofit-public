#pragma once
#include "project.h"

#include "core_component/ComponentFwd.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

class Component;

struct ComponentResolver;
using ObjectManager = component::TypedObjectManager<ComponentResolver>;

///////////////////////////////////////////////////////////////////////////////
}

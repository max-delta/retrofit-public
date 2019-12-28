#pragma once
#include "project.h"

#include "core_component/ComponentFwd.h"
#include "core_state/StateFwd.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::state;

using ComponentID = component::ResolvedComponentType;

class Component;

struct ComponentResolver;
using ObjectManager = component::TypedObjectManager<ComponentResolver>;
using ObjectRef = component::TypedObjectRef<ComponentResolver>;
using MutableObjectRef = component::TypedMutableObjectRef<ComponentResolver>;
using ComponentRef = component::TypedComponentRef<ComponentResolver>;
using MutableComponentRef = component::TypedMutableComponentRef<ComponentResolver>;

///////////////////////////////////////////////////////////////////////////////
}

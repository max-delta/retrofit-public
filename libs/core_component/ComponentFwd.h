#pragma once
#include "rftl/cstdint"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

using ManagerIdentifier = uint16_t;
static constexpr ManagerIdentifier kInvalidManagerIdentifier = 0;

using ObjectIdentifier = uint64_t;
static constexpr ObjectIdentifier kInvalidObjectIdentifier = 0;

using ResolvedComponentType = uint64_t;
static constexpr ResolvedComponentType kInvalidResolvedComponentType = 0;

using ComponentLabel = char const*;

class ObjectRef;
class ComponentRef;
class View;
class ObjectManager;

template<typename TypeResolver>
class TypedObjectManager;

///////////////////////////////////////////////////////////////////////////////
}}

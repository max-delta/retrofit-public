#pragma once

// Forwards
namespace RF::reflect {
	struct ClassInfo;
}

namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
bool IsSameOrDerivedFrom( reflect::ClassInfo const& descendent );

// This relies on VirtualClass in order to walk the inheritance chain, allowing
//  for a type-safe downcast
// WARNING: Multiple inheritance is partially supported, but must be done with
//  non-virtual inheritance for the offsets to resolve correctly. Since the act
//  of multiple-inheriting with all parents deriving from VirtualClass results
//  in a diamond, upcasts will need to be disambiguated. RFType generally
//  assumes the 'left-most' parent was chosen for an upcast, which is an
//  assumption based on implementation-specific behavior, so you're kinda
//  setting yourself up to fail regardless.
// SEE: ISO N4713 C++17 8.5.1.9.2 "... If ... is a virtual base class ..., the
//  program is ill-formed."
template<typename TargetT, typename SourceT>
TargetT virtual_cast( SourceT* source );

///////////////////////////////////////////////////////////////////////////////
}

#include "VirtualCast.inl"

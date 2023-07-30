#pragma once

// Forwards
namespace RF::reflect {
struct ClassInfo;
class VirtualClassWithoutDestructor;
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

// This relies on reflection to convert an unknown pointer into a VirtualClass
//  pointer, generally so that further virtual casts can be performed on it to
//  reach a concrete pointer type that the compiler can work with in normal
//  non-reflection code
reflect::VirtualClassWithoutDestructor const* virtual_reflect_cast(
	reflect::ClassInfo const& classInfo,
	void const* classLocation );
reflect::VirtualClassWithoutDestructor* virtual_reflect_cast(
	reflect::ClassInfo const& classInfo,
	void* classLocation );

///////////////////////////////////////////////////////////////////////////////
}

#include "VirtualCast.inl"

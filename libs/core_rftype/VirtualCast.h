#pragma once

// Forwards
namespace RF { namespace reflect {
	struct ClassInfo;
}}

namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
bool IsSameOrDerivedFrom( reflect::ClassInfo const& descendent );

template<typename TargetT, typename SourceT>
TargetT virtual_cast( SourceT* source );

///////////////////////////////////////////////////////////////////////////////
}}

#include "VirtualCast.inl"

#pragma once

#include "core/ptr/weak_ptr.h"

// Forwards
namespace RF::reflect {
	struct ClassInfo;
}

namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

// See description and warnings on virtual_cast<...>(...)
// NOTE: This has the additional restriction that casts requiring pointer
//  offsets are forbidden
template<typename TargetT, typename SourceT>
WeakPtr<TargetT> virtual_ptr_cast( WeakPtr<SourceT>&& source );

///////////////////////////////////////////////////////////////////////////////
}

#include "VirtualPtrCast.inl"

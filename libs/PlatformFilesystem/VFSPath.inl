#pragma once
#include "VFSPath.h"

namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

template<typename... Nodes>
inline RF::file::VFSPath::VFSPath( Element element, Nodes... elements )
	: SegmentedIdentifier( element, elements... )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}}

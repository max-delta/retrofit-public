#pragma once
#include "VFSPath.h"

namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

template<typename... Nodes>
inline RF::file::VFSPath::VFSPath( Element element, Nodes... elements )
{
	AppendUnroll( element, elements... );
}



template<typename... PathsOrElements>
inline VFSPath RF::file::VFSPath::GetChild( PathsOrElements... pathsOrElements ) const
{
	VFSPath retVal = *this;
	return retVal.AppendUnroll( pathsOrElements... );
}



template<typename... PathsOrElements>
inline VFSPath& RF::file::VFSPath::Append( PathsOrElements... pathsOrElements )
{
	return AppendUnroll( pathsOrElements... );
}

///////////////////////////////////////////////////////////////////////////////

template<typename PathOrElement1, typename PathOrElement2, typename... PathsOrElements>
inline VFSPath& RF::file::VFSPath::AppendUnroll( PathOrElement1 pathOrElement1, PathOrElement2 pathOrElement2, PathsOrElements... pathsOrElements )
{
	AppendUnroll( pathOrElement1 );
	AppendUnroll( pathOrElement2 );
	return AppendUnroll( pathsOrElements... );
}

///////////////////////////////////////////////////////////////////////////////
}}

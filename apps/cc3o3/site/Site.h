#pragma once
#include "cc3o3/site/SiteFwd.h"

#include "PlatformFilesystem/VFSPath.h"


namespace RF::cc::site {
///////////////////////////////////////////////////////////////////////////////

class Site
{
	//
	// Public methods
public:
	Site() = default;

	static Site LoadFromDesc( file::VFSPath const& descPath );


	//
	// Public data
public:
	// TODO
};

///////////////////////////////////////////////////////////////////////////////
}

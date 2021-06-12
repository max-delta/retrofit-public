#include "stdafx.h"
#include "Site.h"

#include "cc3o3/site/SiteDesc.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"


namespace RF::cc::site {
///////////////////////////////////////////////////////////////////////////////

Site Site::LoadFromDesc( file::VFSPath const& descPath )
{
	Site retVal = {};

	UniquePtr<SiteDesc const> descPtr = resource::LoadFromFile<SiteDesc const>( descPath );
	RFLOG_TEST_AND_FATAL( descPtr != nullptr, descPath, RFCAT_CC3O3, "Failed to load site desc" );
	SiteDesc const& desc = *descPtr;

	// TODO
	( (void)desc );
	( (void)retVal );

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

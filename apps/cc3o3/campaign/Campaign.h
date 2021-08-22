#pragma once
#include "cc3o3/campaign/CampaignFwd.h"

#include "PlatformFilesystem/VFSFwd.h"


namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

class Campaign
{
	//
	// Public methods
public:
	Campaign() = default;

	static UniquePtr<Campaign const> LoadFromFolder( file::VFSPath const& campaignPath );


	//
	// Public data
public:
	// TODO
};

///////////////////////////////////////////////////////////////////////////////
}

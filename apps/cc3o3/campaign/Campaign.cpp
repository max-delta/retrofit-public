#include "stdafx.h"
#include "Campaign.h"

#include "cc3o3/campaign/CampaignDesc.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

UniquePtr<Campaign const> Campaign::LoadFromFolder( file::VFSPath const& campaignPath )
{
	UniquePtr<Campaign> retVal = DefaultCreator<Campaign>::Create();
	Campaign& campaign = *retVal;

	// Load campaign desc
	{
		file::VFSPath const& descPath = campaignPath.GetChild( "campaign.oo" );

		UniquePtr<CampaignDesc const> descPtr = resource::LoadFromFile<CampaignDesc const>( descPath );
		RFLOG_TEST_AND_FATAL( descPtr != nullptr, descPath, RFCAT_CC3O3, "Failed to load campaign desc" );
		CampaignDesc const& desc = *descPtr;

		// TODO
		( (void)desc.mPlaceholder );
	}

	// TODO: Lots of other complex data from other files
	( (void)campaign );

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

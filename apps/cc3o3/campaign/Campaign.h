#pragma once
#include "cc3o3/campaign/CampaignFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/string"
#include "rftl/vector"


namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

struct RosterMember
{
	rftl::string mIdentifier;
};



struct Company
{
	rftl::vector<RosterMember> mRoster;
};

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
	Company mPlayerCompany;
};

///////////////////////////////////////////////////////////////////////////////
}

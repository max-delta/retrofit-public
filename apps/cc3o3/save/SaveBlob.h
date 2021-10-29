#pragma once
#include "project.h"

#include "cc3o3/save/SaveFwd.h"

#include "rftl/string"


namespace RF::cc::save {
///////////////////////////////////////////////////////////////////////////////

struct SaveFileBlob final
{
	bool mNewGame = false;
	rftl::string mCampaignName;
};



struct SaveBlob final
{
	SaveFileBlob mFile;
};

///////////////////////////////////////////////////////////////////////////////
}

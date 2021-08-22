#include "stdafx.h"
#include "CampaignDesc.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::cc::campaign::CampaignDesc )
{
	using RF::cc::campaign::CampaignDesc;
	RFTYPE_META().RawProperty( "mPlaceholder", &CampaignDesc::mPlaceholder );
	RFTYPE_REGISTER_BY_NAME( "CampaignDesc" );
}

namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

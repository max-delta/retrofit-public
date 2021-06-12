#include "stdafx.h"
#include "SiteDesc.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::cc::site::SiteDesc )
{
	using RF::cc::site::SiteDesc;
	RFTYPE_META().RawProperty( "mTODO", &SiteDesc::mTODO );
	RFTYPE_REGISTER_BY_NAME( "SiteDesc" );
}

namespace RF::cc::site {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

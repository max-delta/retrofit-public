#include "stdafx.h"
#include "SaveBlob.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::save::SaveFileBlob )
{
	using RF::cc::save::SaveFileBlob;
	RFTYPE_META().RawProperty( "mNewGame", &SaveFileBlob::mNewGame );
	RFTYPE_META().ExtensionProperty( "mCampaignName", &SaveFileBlob::mCampaignName );
	RFTYPE_REGISTER_BY_NAME( "SaveFileBlob" );
}

RFTYPE_CREATE_META( RF::cc::save::SaveBlob )
{
	using RF::cc::save::SaveBlob;
	RFTYPE_META().RawProperty( "mFile", &SaveBlob::mFile );
	RFTYPE_REGISTER_BY_NAME( "SaveBlob" );
}

namespace RF::cc::save {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "SaveBlob.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::cc::save::SaveFileBlob )
{
	RFTYPE_META().RawProperty( "mStub", &RF::cc::save::SaveFileBlob::mNewGame );
	RFTYPE_REGISTER_BY_NAME( "SaveFileBlob" );
}


namespace RF::cc::save {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

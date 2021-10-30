#include "stdafx.h"
#include "RosterMemberDesc.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::campaign::RosterMemberDesc )
{
	using RF::cc::campaign::RosterMemberDesc;
	RFTYPE_META().ExtensionProperty( "mIdentifier", &RosterMemberDesc::mIdentifier );
	RFTYPE_REGISTER_BY_NAME( "RosterMemberDesc" );
}

namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

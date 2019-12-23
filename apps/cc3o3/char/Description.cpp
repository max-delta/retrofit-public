#include "stdafx.h"
#include "Description.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::Description )
{
	using namespace RF::cc::character;
	RFTYPE_META().ExtensionProperty( "mName", &Description::mName );
	RFTYPE_META().ExtensionProperty( "mTitle", &Description::mTitle );
	RFTYPE_REGISTER_BY_NAME( "Description" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::Description );
}


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}}}

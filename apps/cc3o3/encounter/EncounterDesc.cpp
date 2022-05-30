#include "stdafx.h"
#include "EncounterDesc.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"
#include "core_rftype/stl_extensions/vector.h"


RFTYPE_CREATE_META( RF::cc::encounter::EntityDesc )
{
	using RF::cc::encounter::EntityDesc;
	RFTYPE_META().ExtensionProperty( "mCharacterID", &EntityDesc::mCharacterID );
	RFTYPE_REGISTER_BY_NAME( "EntityDesc" );
}

RFTYPE_CREATE_META( RF::cc::encounter::EncounterDesc )
{
	using RF::cc::encounter::EncounterDesc;
	RFTYPE_META().ExtensionProperty( "mEntities", &EncounterDesc::mEntities );
	RFTYPE_REGISTER_BY_NAME( "EncounterDesc" );
}

namespace RF::cc::encounter {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

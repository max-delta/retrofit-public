#include "stdafx.h"
#include "Equipment.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::Equipment )
{
	using namespace RF::cc::character;
	RFTYPE_META().ExtensionProperty( "mArmor1", &Equipment::mArmor1 );
	RFTYPE_META().ExtensionProperty( "mArmor2", &Equipment::mArmor2 );
	RFTYPE_META().ExtensionProperty( "mAccessory1", &Equipment::mAccessory1 );
	RFTYPE_META().ExtensionProperty( "mAccessory2", &Equipment::mAccessory2 );
	RFTYPE_REGISTER_BY_NAME( "Equipment" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::Equipment );
}


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

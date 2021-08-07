#include "stdafx.h"
#include "Genetics.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::Genetics )
{
	using namespace RF::cc::character;
	RFTYPE_META().ExtensionProperty( "mSpecies", &Genetics::mSpecies );
	RFTYPE_META().ExtensionProperty( "mGender", &Genetics::mGender );
	RFTYPE_REGISTER_BY_NAME( "Genetics" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::Genetics );
}


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

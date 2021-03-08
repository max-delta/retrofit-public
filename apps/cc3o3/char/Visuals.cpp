#include "stdafx.h"
#include "Visuals.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::Visuals )
{
	using namespace RF::cc::character;
	RFTYPE_META().ExtensionProperty( "mBase", &Visuals::mBase );
	RFTYPE_META().ExtensionProperty( "mTop", &Visuals::mTop );
	RFTYPE_META().ExtensionProperty( "mBottom", &Visuals::mBottom );
	RFTYPE_META().ExtensionProperty( "mHair", &Visuals::mHair );
	RFTYPE_META().ExtensionProperty( "mSpecies", &Visuals::mSpecies );
	RFTYPE_REGISTER_BY_NAME( "Visuals" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::Visuals );
}


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

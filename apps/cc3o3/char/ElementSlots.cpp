#include "stdafx.h"
#include "ElementSlots.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/array.h"


RFTYPE_CREATE_META( RF::cc::character::ElementSlots )
{
	using namespace RF::cc::character;
	RFTYPE_META().ExtensionProperty( "mGrid", &ElementSlots::mGrid );
	RFTYPE_REGISTER_BY_NAME( "ElementSlots" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::ElementSlots );
}


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

ElementSlots::Slot const& ElementSlots::At( ElementSlotIndex const& location ) const
{
	size_t const levelOffset = element::AsLevelOffset( location.first );
	return mGrid.at( levelOffset ).at( location.second );
}



ElementSlots::Slot& ElementSlots::At( ElementSlotIndex const& location )
{
	size_t const levelOffset = element::AsLevelOffset( location.first );
	return mGrid.at( levelOffset ).at( location.second );
}

///////////////////////////////////////////////////////////////////////////////
}

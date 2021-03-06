#include "stdafx.h"
#include "ElementSlots.h"

#include "RFType/CreateClassInfoDefinition.h"

// TODO: Add an extension for this
//#include "core_rftype/stl_extensions/array.h"
#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::ElementSlots )
{
	using namespace RF::cc::character;
	// TODO: Add an extension for this (rftl::array<...>)
	//RFTYPE_META().ExtensionProperty( "mGrid", &ElementSlots::mGrid );
	RFTYPE_REGISTER_BY_NAME( "ElementSlots" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::ElementSlots );
}


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

ElementSlots::Slot const& ElementSlots::At( ElementSlotIndex const& location ) const
{
	return mGrid.at( location.first ).at( location.second );
}



ElementSlots::Slot& ElementSlots::At( ElementSlotIndex const& location )
{
	return mGrid.at( location.first ).at( location.second );
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "Loadout.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

character::ElementSlots Loadout::CalcElements() const
{
	character::ElementSlots retVal = mEquippedElements;

	// TODO: Re-design how innate elements are stored
	( (void)mInnateElements );

	return retVal;
}



void Loadout::AssignEquippedElement( character::ElementSlotIndex slot, element::ElementIdentifier element )
{
	RF_ASSERT( slot.first != element::kInvalidElementLevel );
	RF_ASSERT( slot.first >= element::kMinElementLevel );
	RF_ASSERT( slot.first <= element::kMaxElementLevel );
	mEquippedElements.At( slot ) = element;
}

///////////////////////////////////////////////////////////////////////////////
}

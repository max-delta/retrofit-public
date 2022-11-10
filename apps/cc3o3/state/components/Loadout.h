#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"
#include "cc3o3/char/Equipment.h"
#include "cc3o3/char/ElementSlots.h"
#include "cc3o3/char/GridMask.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class Loadout final : public state::Component
{
	RF_NO_COPY( Loadout );

	//
	// Public methods
public:
	Loadout() = default;

	// TODO: Re-design how innate elements are stored
	character::ElementSlots CalcElements() const;

	void AssignEquippedElement( character::ElementSlotIndex slot, element::ElementIdentifier element );


	//
	// Public data
public:
	character::Equipment mEquipment = {};

	character::GridMask mGridMask = {};

	// TODO: Re-design how innate elements are stored
	character::ElementSlots mInnateElements = {};
	character::ElementSlots mEquippedElements = {};
};

///////////////////////////////////////////////////////////////////////////////
}

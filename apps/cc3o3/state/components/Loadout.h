#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"
#include "cc3o3/char/Equipment.h"
#include "cc3o3/char/ElementSlots.h"

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


	//
	// Public data
public:
	character::Equipment mEquipment = {};
	// TODO: Re-design how innate elements are stored
	character::ElementSlots mInnateElements = {};
	character::ElementSlots mEquippedElements = {};
};

///////////////////////////////////////////////////////////////////////////////
}

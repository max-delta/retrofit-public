#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"
#include "cc3o3/char/CharData.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class Character final : public state::Component
{
	RF_NO_COPY( Character );

	//
	// Public methods
public:
	Character() = default;


	//
	// Public data
public:
	character::CharData mCharData = {};
};

///////////////////////////////////////////////////////////////////////////////
}

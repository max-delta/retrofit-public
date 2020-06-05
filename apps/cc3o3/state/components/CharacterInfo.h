#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"
#include "cc3o3/char/Character.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class CharacterInfo final : public state::Component
{
	RF_NO_COPY( CharacterInfo );

	//
	// Public methods
public:
	CharacterInfo() = default;


	//
	// Public data
public:
	character::Character mCharacter = {};
};

///////////////////////////////////////////////////////////////////////////////
}

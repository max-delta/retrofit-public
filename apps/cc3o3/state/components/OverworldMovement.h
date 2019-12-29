#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

#include "core_math/Vector2.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class OverworldMovement final : public state::Component
{
	RF_NO_COPY( OverworldMovement );

	//
	// Structs
public:
	struct Pos
	{
		math::Vector2i16 mPos = {};
		enum Facing : uint8_t
		{
			North = 0,
			East,
			South,
			West,
		} mFacing = North;
		bool mMoving = false;
	};


	//
	// Public methods
public:
	OverworldMovement() = default;

	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override;


	//
	// Public data
public:
	Pos mCurPos = {};
};

///////////////////////////////////////////////////////////////////////////////
}

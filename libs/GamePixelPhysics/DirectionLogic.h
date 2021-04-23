#pragma once
#include "project.h"

#include "GamePixelPhysics/PhysicsFwd.h"

#include "core/macros.h"


namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////

// Intended to be used with input systems that use start/stop cardinal commands
class GAMEPIXELPHYSICS_API DirectionLogic
{
	RF_NO_INSTANCE( DirectionLogic );

	//
	// Public methods
public:
	// SEE: Direction::kConflicts
	static bool HasConflicts( Direction::Value dir );

	// A clean stack is formed of the following:
	//  * Primary - A single cardinal
	//  * Secondary - An optional single cardinal that could combine with the
	//      primary to create an intercardinal
	//  * Latent - Any remaining directional bits that create conflicts
	static void CleanStack(
		Direction::Value& primary,
		Direction::Value& secondary,
		Direction::Value& latent );

	// Update a stack based on new direction changes
	// NOTE: Performs stack cleaning functionality as well
	// SEE: CleanStack(...)
	static void UpdateStack(
		Direction::Value& primary,
		Direction::Value& secondary,
		Direction::Value& latent,
		Direction::Value adds,
		Direction::Value removes );
};

///////////////////////////////////////////////////////////////////////////////
}

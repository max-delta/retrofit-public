#pragma once
#include "project.h"

namespace RF::novel {
///////////////////////////////////////////////////////////////////////////////

namespace CinematicActions {
enum Value : uint8_t
{
	Invalid = 0,

	// Advance the cinematic past the current point if it is waiting
	Advance = 1 << 0,
};
}

enum class CinematicState : uint8_t
{
	Invalid = 0,

	// Nothing transitionary is happening, waiting for a signal to advance
	WaitingForAdvance,

	// At the end of the cinematic, nothing else to do
	EndOfSequence,
};

class CinematicDriver;

///////////////////////////////////////////////////////////////////////////////
}

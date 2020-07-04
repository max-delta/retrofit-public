#pragma once
#include "project.h"

#include "GameInput/InputFwd.h"

namespace RF { namespace cc { namespace input {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::input;

namespace player {
enum : PlayerID
{
	Invalid = kInvalidPlayerID,
	P1,
	P2
};
}

namespace layer {
enum : LayerID
{
	Invalid = kInvalidLayerID,
	MainMenu,
	GameMenu,
	CharacterControl,

	Developer,
};
}

namespace command {
namespace raw {
enum : RawCommandType
{
	Invalid = kInvalidRawCommand,

	Up,
	UpStop,
	Down,
	DownStop,
	Left,
	LeftStop,
	Right,
	RightStop,

	HatUp,
	HatUpStop,
	HatDown,
	HatDownStop,
	HatLeft,
	HatLeftStop,
	HatRight,
	HatRightStop,

	PgUp,
	PgDn,
	Home,
	End,

	Affirmative,
	Negative,
	Auxiliary,

	DeveloperToggle,
	DeveloperCycle,
	DeveloperAction1,
	DeveloperAction2,
	DeveloperAction3,
};
}
namespace game {
enum : GameCommandType
{
	Invalid = kInvalidGameCommand,

	UINavigateUp,
	UINavigateDown,
	UINavigateLeft,
	UINavigateRight,
	UINavigateToPreviousGroup,
	UINavigateToNextGroup,
	UINavigateToFirst,
	UINavigateToLast,
	UIActivateSelection,
	UICancelSelection,

	WalkNorth,
	WalkNorthStop,
	WalkEast,
	WalkEastStop,
	WalkSouth,
	WalkSouthStop,
	WalkWest,
	WalkWestStop,

	DeveloperToggle,
	DeveloperCycle,
	DeveloperAction1,
	DeveloperAction2,
	DeveloperAction3,
};
}
}

///////////////////////////////////////////////////////////////////////////////
}}}

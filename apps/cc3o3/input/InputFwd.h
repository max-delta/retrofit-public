#pragma once
#include "project.h"

#include "GameInput/InputFwd.h"

namespace RF::cc::input {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::input;

namespace player {
enum : PlayerID
{
	Invalid = kInvalidPlayerID,
	P1,
	P2,
	Global = kMaxPlayerID,
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
	Auxiliary1,
	Auxiliary2,

	GameSelect, // Historical name, generally opens menus
	GameStart, // Historical name, generally pauses game

	DeveloperToggle,
	DeveloperCycle,
	DeveloperAction1,
	DeveloperAction2,
	DeveloperAction3,
	DeveloperAction4,
	DeveloperAction5,
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
	UIAuxiliaryAction1,
	UIAuxiliaryAction2,
	UIMenuAction,
	UIPauseAction,

	WalkNorth,
	WalkNorthStop,
	WalkEast,
	WalkEastStop,
	WalkSouth,
	WalkSouthStop,
	WalkWest,
	WalkWestStop,
	Interact,

	DeveloperToggle,
	DeveloperCycle,
	DeveloperAction1,
	DeveloperAction2,
	DeveloperAction3,
	DeveloperAction4,
	DeveloperAction5,
};
}
}

///////////////////////////////////////////////////////////////////////////////
}

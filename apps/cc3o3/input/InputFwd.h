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
	P1
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
	Down,
	Left,
	Right,

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
	UINavigateToFirst,
	UINavigateToLast,
	UIActivateSelection,
	UICancelSelection,

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

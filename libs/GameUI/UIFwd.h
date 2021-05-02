#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/unordered_set"
#include "rftl/vector"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

enum class Orientation : uint8_t
{
	Horizontal = 0,
	Vertical
};

enum class Justification : uint8_t
{
	Invalid = 0,

	Top = 1 << 3,
	Middle = 1 << 4,
	Bottom = 1 << 5,

	Left = 1 << 0,
	Center = 1 << 1,
	Right = 1 << 2,

	TopLeft = Top | Left,
	TopCenter = Top | Center,
	TopRight = Top | Right,
	MiddleLeft = Middle | Left,
	MiddleCenter = Middle | Center,
	MiddleRight = Middle | Right,
	BottomLeft = Bottom | Left,
	BottomCenter = Bottom | Center,
	BottomRight = Bottom | Right
};

using FocusEventType = uint64_t;
namespace focusevent {
enum : FocusEventType
{
	Invalid = 0,

	Notification_FocusLost,
	Notification_FocusGained,

	Command_NavigateUp,
	Command_NavigateDown,
	Command_NavigateLeft,
	Command_NavigateRight,

	Command_NavigateToPreviousGroup,
	Command_NavigateToNextGroup,
	Command_NavigateToFirst,
	Command_NavigateToLast,

	// Custom types should start at this value
	BEGIN_CUSTOM_TYPES
};
}

using ContainerID = uint64_t;
static constexpr ContainerID kInvalidContainerID = 0;
static constexpr ContainerID kRootContainerID = 1;
using ContainerIDList = rftl::vector<ContainerID>;
using ContainerIDSet = rftl::unordered_set<ContainerID>;

using AnchorID = uint64_t;
static constexpr AnchorID kInvalidAnchorID = 0;
using AnchorIDList = rftl::vector<AnchorID>;
using AnchorIDSet = rftl::unordered_set<AnchorID>;

using FontPurposeID = uint16_t;
static constexpr FontPurposeID kInvalidFontPurposeID = 0;

class ContainerManager;
class Controller;
class FontRegistry;
class FocusTree;
class FocusManager;

struct Container;
struct Anchor;
struct FocusKey;
struct FocusEvent;
struct FocusTarget;
struct FocusTreeNode;
struct Font;
struct UIContext;
struct UIConstContext;

// NOTE: Need to declare namespace before aliasing or wrapping
namespace controller {
};

///////////////////////////////////////////////////////////////////////////////
}

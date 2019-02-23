#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/unordered_set"
#include "rftl/vector"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

enum class Justification : uint8_t
{
	Top = 1 << 3,
	Middle = 2 << 3,
	Bottom = 4 << 3,

	Left = 1,
	Center = 2,
	Right = 4,

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

struct Container;
struct Anchor;

///////////////////////////////////////////////////////////////////////////////
}}

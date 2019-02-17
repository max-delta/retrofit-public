#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API Anchor
{
	RF_NO_COPY( Anchor );

	Anchor() = default;

	// Anchors have no direct relationship to each other
	AnchorID mAnchorID = kInvalidAnchorID;

	// Anchors are owned by containers
	ContainerID mParentContainerID = kInvalidContainerID;

	// Anchors are dimensionless points
	gfx::PPUCoord mPos;
};

///////////////////////////////////////////////////////////////////////////////
}}

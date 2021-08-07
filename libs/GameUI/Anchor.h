#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "core_math/Vector2.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API Anchor
{
	RF_NO_COPY( Anchor );

	//
	// Constants
public:
	static constexpr gfx::ppu::CoordElem kInvalidPosElem = rftl::numeric_limits<gfx::ppu::CoordElem>::min();


	//
	// Public methods
public:
	Anchor() = default;

	void InvalidatePosition();
	bool HasValidPosition() const;


	//
	// Public data
public:
	// Anchors have no direct relationship to each other
	AnchorID mAnchorID = kInvalidAnchorID;

	// Anchors are owned by containers
	ContainerID mParentContainerID = kInvalidContainerID;

	// Anchors are dimensionless points
	gfx::ppu::Coord mPos = { kInvalidPosElem, kInvalidPosElem };
};

///////////////////////////////////////////////////////////////////////////////
}

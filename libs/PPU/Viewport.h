#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core_math/Vector2.h"


// Forwards
namespace RF::math {
template<typename T>
class AABB4;
}

namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Viewport
{
	//
	// Public methods
public:
	Viewport() = default;
	~Viewport() = default;

	void SlideToFit( ppu::Coord pos, ppu::CoordElem xMargin, ppu::CoordElem yMargin );

	void ClampToWithin( ppu::AABB aabb );

	//
	// Public data
public:
	ppu::Coord mOffset = {};
	ppu::Vec2 mSurfaceExtents = {};
};

///////////////////////////////////////////////////////////////////////////////
}

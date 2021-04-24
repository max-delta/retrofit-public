#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core_math/Vector2.h"


// Forwards
namespace RF::math {
template<typename T>
class AABB4;
}

namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Viewport
{
	//
	// Public methods
public:
	Viewport() = default;
	~Viewport() = default;

	void SlideToFit( PPUCoord pos, PPUCoordElem xMargin, PPUCoordElem yMargin );

	void ClampToWithin( math::AABB4<PPUCoordElem> aabb );

	//
	// Public data
public:
	PPUCoord mOffset = {};
	PPUVec mSurfaceExtents = {};
};

///////////////////////////////////////////////////////////////////////////////
}

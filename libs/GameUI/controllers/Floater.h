#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Vector2.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Creates a 'floating' container that is not strictly bound by its parent
//  container's AABB, desirable for things like pop-ups
class GAMEUI_API Floater final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( Floater );

	//
	// Public methods
public:
	Floater() = delete;
	Floater( gfx::ppu::CoordElem width, gfx::ppu::CoordElem height, Justification::Value justification );

	ContainerID GetChildContainerID() const;

	gfx::ppu::Coord GetOffset() const;
	void SetOffset( UIContext& context, gfx::ppu::Coord offset );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	AnchorID mTopLeftAnchor = kInvalidAnchorID;
	AnchorID mBottomRightAnchor = kInvalidAnchorID;
	ContainerID mContainer = kInvalidContainerID;
	Justification::Value mJustification = Justification::TopLeft;
	gfx::ppu::Coord mDimensions = {};
	gfx::ppu::Coord mOffset = {};
};

///////////////////////////////////////////////////////////////////////////////
}

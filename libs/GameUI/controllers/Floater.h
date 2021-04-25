#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Vector2.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Floater final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( Floater );

	//
	// Public methods
public:
	Floater() = delete;
	Floater( gfx::ppu::PPUCoordElem width, gfx::ppu::PPUCoordElem height, Justification justification );

	ContainerID GetChildContainerID() const;

	gfx::ppu::PPUCoord GetOffset() const;
	void SetOffset( UIContext& context, gfx::ppu::PPUCoord offset );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	AnchorID mTopLeftAnchor = kInvalidAnchorID;
	AnchorID mBottomRightAnchor = kInvalidAnchorID;
	ContainerID mContainer = kInvalidContainerID;
	Justification mJustification = Justification::TopLeft;
	gfx::ppu::PPUCoord mDimensions = {};
	gfx::ppu::PPUCoord mOffset = {};
};

///////////////////////////////////////////////////////////////////////////////
}

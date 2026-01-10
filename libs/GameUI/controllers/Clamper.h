#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Vector2.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Reduces down the size of a container, generally to add padding or to enforce
//  certain restrictions on sub-containers such as max size or tile alignment
class GAMEUI_API Clamper final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( Clamper );

	//
	// Structs
public:
	// A coord value of zero causes that aspect to not be used in clamping
	// NOTE: Order of operations is subtract -> clamp to max -> snap to low
	struct Params
	{
		gfx::ppu::CoordElem subtractWidth = 0;
		gfx::ppu::CoordElem subtractHeight = 0;
		gfx::ppu::CoordElem maxWidth = 0;
		gfx::ppu::CoordElem maxHeight = 0;
		gfx::ppu::CoordElem divisibleByWidth = 0;
		gfx::ppu::CoordElem divisibleByHeight = 0;
	};


	//
	// Public methods
public:
	Clamper() = delete;
	Clamper(
		Params params,
		Justification::Value justification );

	ContainerID GetChildContainerID() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	AnchorID mTopLeftAnchor = kInvalidAnchorID;
	AnchorID mBottomRightAnchor = kInvalidAnchorID;
	ContainerID mContainer = kInvalidContainerID;
	Justification::Value const mJustification = Justification::TopLeft;
	gfx::ppu::Coord const mSubtractDimensions = {};
	gfx::ppu::Coord const mMaxDimensions = {};
	gfx::ppu::Coord const mStepDimensions = {};
};

///////////////////////////////////////////////////////////////////////////////
}

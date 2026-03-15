#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Vector2.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Cuts a container into two, using a fixed size and a single horizontal or
//  vertical slice
class GAMEUI_API ClampSlicer final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ClampSlicer );

	//
	// Types and constants
public:
	enum class Mode : uint8_t
	{
		Invalid = 0,

		// Quirky, this functionally disables the slicer, and is intended for
		//  use cases where you intend to shut off one of the child containers,
		//  which conceptually 'maximizes' the other container
		TotalOverlap,

		// Columns
		ClampLeft_OverflowRight,
		ClampRight_OverflowLeft,

		// Rows
		ClampTop_OverflowBottom,
		ClampBottom_OverflowTop,
	};


	//
	// Structs
public:
	// A coord value of zero causes that aspect to not be used in clamping
	// NOTE: Order of operations is subtract -> clamp to max -> snap to low
	struct Params
	{
		gfx::ppu::CoordElem subtract = 0;
		gfx::ppu::CoordElem max = 0;
		gfx::ppu::CoordElem divisibleBy = 0;
	};


	//
	// Public methods
public:
	ClampSlicer() = delete;
	ClampSlicer(
		Params params,
		Mode mode );

	void SetMode( ui::UIContext& context, Mode mode );

	ContainerID GetClampedContainerID() const;
	ContainerID GetOverflowContainerID() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	AnchorID mClampedTopLeftAnchor = kInvalidAnchorID;
	AnchorID mClampedBottomRightAnchor = kInvalidAnchorID;
	AnchorID mOverflowTopLeftAnchor = kInvalidAnchorID;
	AnchorID mOverflowBottomRightAnchor = kInvalidAnchorID;
	ContainerID mClampedContainer = kInvalidContainerID;
	ContainerID mOverflowContainer = kInvalidContainerID;
	Mode mMode = Mode::Invalid;
	gfx::ppu::CoordElem const mSubtract = {};
	gfx::ppu::CoordElem const mMax = {};
	gfx::ppu::CoordElem const mStep = {};
};

///////////////////////////////////////////////////////////////////////////////
}

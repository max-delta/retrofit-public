#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Ratio.h"

#include "rftl/array"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Splits into three columns, with the center column trying to take up as much
//  space as possible, up until it hits a maximum width as defined by a target
//  aspect ratio, after which it will center itself while the side columns grow
// 	to take up the slack
// NOTE: Intent is keep ultra-hyper-super-widescreen viewports from resulting
//  in overly wide layouts that break the intended / practical design limits
class GAMEUI_API AspectColumnSlicer final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( AspectColumnSlicer );

	//
	// Types
public:
	using Enableds = rftl::array<bool, 3>;

	// Expressed in width / height
	// Notable ratios and resolutions:
	//  * 10/9 - Game Boy, QQVGA (160x144)
	//  * 3/2 - Game Boy Advance, HQVGA (240x160)
	//  * 8/7 - Super Nintendo standard resolution without overscan (256x224)
	//  * 8/5 - DOS, CGA (320x200)
	//  * 12/7 - RetroFit recommended WQVGA-like with 32-pixel tiles (384x224)
	//  * 4/3 - Legacy resolution, SVGA (800x600)
	//  * 16/9 - Modern resolution, FHD (1920x1080)
	using Ratio = math::Ratio32;

private:
	// The resolution from the UI layout system cannot exceed the maximum
	//  expressable ratio dimensions, meaning that at time of writing a 16-bit
	//  interface will limit this to resolutions of 65535x65535, which should
	//  already be quite excessive
	static constexpr size_t kMaxResolutionDimension = rftl::numeric_limits<Ratio::InterfaceType>::max();

	// A center slice, and two slices on each size
	static constexpr size_t kNumSlices = 3;


	//
	// Public methods
public:
	AspectColumnSlicer() = delete;
	AspectColumnSlicer( Ratio maxWideness, Enableds enabledSlices );

	ContainerID GetChildContainerID( size_t sliceIndex ) const;
	ContainerID CreateChildContainer( ContainerManager& manager, size_t sliceIndex );
	void DestroyChildContainer( ContainerManager& manager, size_t sliceIndex );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private methods
private:
	ContainerID CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex );


	//
	// Private data
private:
	ContainerID mParentContainerID = kInvalidContainerID;
	rftl::array<AnchorID, 4> mAnchors = { kInvalidAnchorID, kInvalidAnchorID, kInvalidAnchorID, kInvalidAnchorID };
	rftl::array<ContainerID, 3> mContainers = { kInvalidContainerID, kInvalidContainerID, kInvalidContainerID };
	Enableds mEnabledSlices;
	Ratio mMaxWideness;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "core_math/AABB4.h"
#include "core/ptr/weak_ptr.h"
#include "core/ptr/unique_ptr.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API Container
{
	RF_NO_COPY( Container );

	//
	// Public methods
public:
	Container() = default;

	bool IsConstrainedBy( AnchorID anchorID ) const;

	void OnAssign( UIContext& context );
	void OnAABBRecalc( UIContext& context );
	void OnZoomFactorChange( UIContext& context );
	void OnImminentDestruction( UIContext& context ) const;
	void OnRender( UIConstContext const& context, bool& blockChildRendering ) const;


	//
	// Public data
public:
	// Containers exist in a hierarchy
	ContainerID mContainerID = kInvalidContainerID;
	ContainerID mParentContainerID = kInvalidContainerID;
	ContainerIDList mChildContainerIDs = {};

	// Containers are constrained by anchor points
	// NOTE: Not necessarily to those of their immediate parent, which further
	//  means that a child AABB may be larger than the AABB of its immediate
	//  parent
	AnchorID mLeftConstraint = kInvalidAnchorID;
	AnchorID mRightConstraint = kInvalidAnchorID;
	AnchorID mTopConstraint = kInvalidAnchorID;
	AnchorID mBottomConstraint = kInvalidAnchorID;
	gfx::ppu::AABB mAABB;

	// Containers may own anchor points, which descendants may constrain
	//  themselves to
	AnchorIDList mAnchorIDs;

	// Depth is calculated automatically, but some special cases may need to
	//  adjust the behavior slightly
	// NOTE: This recursively affects all children as well
	gfx::ppu::PPUDepthLayer mDepthOffset = 0;

	// Containers may have a controller that runs their logic
	// NOTE: Possible to only have a weak reference, such as when sharing
	//  controllers that are designed to manage multiple containers
	UniquePtr<Controller> mStrongUIController;
	WeakPtr<Controller> mWeakUIController;
};

///////////////////////////////////////////////////////////////////////////////
}

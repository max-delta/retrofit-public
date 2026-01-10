#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Slices into multiple rows
class GAMEUI_API RowSlicer final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( RowSlicer );

	//
	// Types
public:
	using Ratios = rftl::vector<rftl::pair<float, bool>>;


	//
	// Public methods
public:
	RowSlicer() = delete;
	RowSlicer( Ratios const& ratios );
	RowSlicer( size_t numSlices );

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
	rftl::vector<AnchorID> mAnchors;
	rftl::vector<ContainerID> mContainers;
	Ratios mRatios;
};

///////////////////////////////////////////////////////////////////////////////
}

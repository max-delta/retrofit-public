#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API ColumnSlicer final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ColumnSlicer );

	//
	// Types
public:
	using Ratios = rftl::vector<rftl::pair<float, bool>>;


	//
	// Public methods
public:
	ColumnSlicer() = delete;
	ColumnSlicer( Ratios const& ratios );
	ColumnSlicer( size_t numSlices );

	ContainerID GetChildContainerID( size_t sliceIndex ) const;
	void CreateChildContainer( ContainerManager& manager, size_t sliceIndex );
	void DestroyChildContainer( ContainerManager& manager, size_t sliceIndex );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private methods
private:
	void CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex );


	//
	// Private data
private:
	ContainerID mParentContainerID = kInvalidContainerID;
	rftl::vector<AnchorID> mAnchors;
	rftl::vector<ContainerID> mContainers;
	Ratios mRatios;
};

///////////////////////////////////////////////////////////////////////////////
}}}

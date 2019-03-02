#pragma once
#include "project.h"

#include "GameUI/Controller.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API ColumnSlicer final : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Types
public:
	using Ratios = rftl::vector<rftl::pair<float, bool>>;


	//
	// Public methods
public:
	ColumnSlicer() = delete;
	ColumnSlicer( Ratios const& ratios );

	ContainerID GetChildContainerID( size_t sliceIndex ) const;
	void CreateChildContainer( ContainerManager& manager, size_t sliceIndex );
	void DestroyChildContainer( ContainerManager& manager, size_t sliceIndex );

	virtual void OnAssign( UIContext& context, Container& container ) override;
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

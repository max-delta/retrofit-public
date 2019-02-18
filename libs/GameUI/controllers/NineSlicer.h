#pragma once
#include "project.h"

#include "GameUI/Controller.h"

#include "rftl/array"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

// 0 1 2
// 3 4 5
// 6 7 8
class GAMEUI_API NineSlicer final : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	NineSlicer();
	NineSlicer( bool const ( &sliceEnabled )[9] );

	ContainerID GetChildContainerID( size_t sliceIndex ) const;
	void CreateChildContainer( ContainerManager& manager, size_t sliceIndex );
	void DestroyChildContainer( ContainerManager& manager, size_t sliceIndex );

	virtual void OnAssign( ContainerManager& manager, Container& container ) override;
	virtual void OnAABBRecalc( ContainerManager& manager, Container& container ) override;


	//
	// Private methods
private:
	void CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex );


	//
	// Private data
private:
	ContainerID mParentContainerID = kInvalidContainerID;
	AnchorID m0 = kInvalidAnchorID;
	AnchorID m33 = kInvalidAnchorID;
	AnchorID m66 = kInvalidAnchorID;
	AnchorID m100 = kInvalidAnchorID;
	rftl::array<ContainerID, 9> mContainers = {};
	bool mSliceEnabled[9] = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}

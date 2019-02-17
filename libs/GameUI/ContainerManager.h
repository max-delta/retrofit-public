#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/unordered_map"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API ContainerManager
{
	RF_NO_COPY( ContainerManager );

public:
	using ContainerStorage = rftl::unordered_map<ContainerID, Container>;
	using AnchorStorage = rftl::unordered_map<AnchorID, Anchor>;

	WeakPtr<gfx::PPUController> mGraphics;

	ContainerStorage mContainers;
	AnchorStorage mAnchors;

	ContainerID mLastContainerID = kInvalidContainerID;
	AnchorID mLastAnchorID = kInvalidAnchorID;

	AnchorIDSet mRecalcsNeeded;

	// TODO: Use actual value
	size_t mMaxDepthSeen = 30;

	bool mIsDestroyingContainers = false;


	ContainerManager( WeakPtr<gfx::PPUController> const& ppuController );
	~ContainerManager();

	void CreateRootContainer();
	Container& GetMutableRootContainer();
	void RecalcRootContainer();

	ContainerID CreateChildContainer(
		Container& parentContainer,
		AnchorID leftConstraint,
		AnchorID rightConstraint,
		AnchorID topConstraint,
		AnchorID bottomConstraint );

	Container const& GetContainer( ContainerID containerID ) const;
	Container& GetMutableContainer( ContainerID containerID );
	void RecalcContainer( Container& container );
	void DestroyContainer( ContainerID containerID );

	AnchorID CreateAnchor( Container& container );
	void MoveAnchor( AnchorID anchorID, gfx::PPUCoord pos );
	void DestroyAnchor( AnchorID anchorID );

	template<typename T>
	WeakPtr<T> AssignStrongController( Container& container, CreationPayload<T>&& controller )
	{
		UniquePtr<T> temp = rftl::move( controller );
		WeakPtr<T> const retVal = AssignStrongController<T>( container, rftl::move( temp ) );
		return retVal;
	}



	template<typename T>
	WeakPtr<T> AssignStrongController( Container& container, UniquePtr<T>&& controller )
	{
		WeakPtr<T> const retVal = controller;
		UniquePtr<UIController> temp = rftl::move( controller );
		WeakPtr<UIController> const result = AssignStrongControllerInternal( container, rftl::move( temp ) );
		RF_ASSERT( retVal == result );
		return retVal;
	}

	WeakPtr<UIController> AssignStrongControllerInternal( Container& container, UniquePtr<UIController>&& controller );

	void DebugRender() const;

	void MarkForRecalc( AnchorID anchorID );
	void ProcessRecalcs();

	void ProcessDestruction( ContainerIDSet&& seedContainers, AnchorIDSet&& seedAnchors );
};

///////////////////////////////////////////////////////////////////////////////
}}

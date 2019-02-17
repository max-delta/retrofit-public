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

	//
	// Friends
private:
	friend class Controller;


	//
	// Types
private:
	using ContainerStorage = rftl::unordered_map<ContainerID, Container>;
	using AnchorStorage = rftl::unordered_map<AnchorID, Anchor>;


	//
	// Public methods
public:
	ContainerManager( WeakPtr<gfx::PPUController> const& ppuController );
	~ContainerManager();

	void CreateRootContainer();

private:
	Container& GetMutableRootContainer();

public:
	void RecalcRootContainer();

private:
	ContainerID CreateChildContainer(
		Container& parentContainer,
		AnchorID leftConstraint,
		AnchorID rightConstraint,
		AnchorID topConstraint,
		AnchorID bottomConstraint );

public:
	Container const& GetContainer( ContainerID containerID ) const;

private:
	Container& GetMutableContainer( ContainerID containerID );

	void RecalcContainer( Container& container );
	void DestroyContainer( ContainerID containerID );

	AnchorID CreateAnchor( Container& container );
	void MoveAnchor( AnchorID anchorID, gfx::PPUCoord pos );
	void DestroyAnchor( AnchorID anchorID );

public:
	template<typename T>
	WeakPtr<T> AssignStrongController( ContainerID containerID, CreationPayload<T>&& controller )
	{
		return AssignStrongController( GetMutableContainer( containerID ), rftl::move( controller ) );
	}

	template<typename T>
	WeakPtr<T> AssignStrongController( ContainerID containerID, UniquePtr<T>&& controller )
	{
		return AssignStrongController( GetMutableContainer( containerID ), rftl::move( controller ) );
	}

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
		UniquePtr<Controller> temp = rftl::move( controller );
		WeakPtr<Controller> const result = AssignStrongControllerInternal( container, rftl::move( temp ) );
		RF_ASSERT( retVal == result );
		return retVal;
	}

private:
	WeakPtr<Controller> AssignStrongControllerInternal( Container& container, UniquePtr<Controller>&& controller );

public:
	void DebugRender() const;

private:
	void MarkForRecalc( AnchorID anchorID );

public:
	void ProcessRecalcs();

private:
	void ProcessDestruction( ContainerIDSet&& seedContainers, AnchorIDSet&& seedAnchors );


	//
	// Private data
private:
	WeakPtr<gfx::PPUController> mGraphics;

	ContainerStorage mContainers;
	AnchorStorage mAnchors;

	ContainerID mLastContainerID = kInvalidContainerID;
	AnchorID mLastAnchorID = kInvalidAnchorID;

	AnchorIDSet mRecalcsNeeded;

	// TODO: Use actual value
	size_t mMaxDepthSeen = 30;

	bool mIsDestroyingContainers = false;
};

///////////////////////////////////////////////////////////////////////////////
}}

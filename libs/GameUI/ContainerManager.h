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
	void RecalcRootContainer();

	Container const& GetContainer( ContainerID containerID ) const;

	template<typename T>
	WeakPtr<T> AssignStrongController( ContainerID containerID, CreationPayload<T>&& controller );
	template<typename T>
	WeakPtr<T> AssignStrongController( ContainerID containerID, UniquePtr<T>&& controller );
	template<typename T>
	WeakPtr<T> AssignStrongController( Container& container, CreationPayload<T>&& controller );
	template<typename T>
	WeakPtr<T> AssignStrongController( Container& container, UniquePtr<T>&& controller );

	void SetRootRenderDepth( gfx::PPUDepthLayer depth );
	gfx::PPUDepthLayer GetRecommendedRenderDepth( Container const& container ) const;

	void ProcessRecalcs();

	void Render() const;

	void DebugRender() const;


	//
	// Private methods
private:
	gfx::PPUController& GetRenderer() const;

	Container& GetMutableRootContainer();

	ContainerID CreateChildContainer(
		Container& parentContainer,
		AnchorID leftConstraint,
		AnchorID rightConstraint,
		AnchorID topConstraint,
		AnchorID bottomConstraint );

	Container& GetMutableContainer( ContainerID containerID );

	void RecalcContainer( Container& container );
	void DestroyContainer( ContainerID containerID );

	AnchorID CreateAnchor( Container& container );
	void MoveAnchor( AnchorID anchorID, gfx::PPUCoord pos );
	void DestroyAnchor( AnchorID anchorID );

	WeakPtr<Controller> AssignStrongControllerInternal( Container& container, UniquePtr<Controller>&& controller );

	void MarkForRecalc( AnchorID anchorID );

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

	gfx::PPUDepthLayer mRootRenderDepth = 0;

	bool mIsDestroyingContainers = false;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "ContainerManager.inl"

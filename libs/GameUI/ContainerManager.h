#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "core/ptr/weak_ptr.h"
#include "core/ptr/unique_ptr.h"

#include "rftl/extension/string_hash.h"
#include "rftl/unordered_map"
#include "rftl/string"


namespace RF::ui {
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
	using LabelToContainerID = rftl::unordered_map<rftl::string, ContainerID, rftl::string_hash, rftl::equal_to<>>;


	//
	// Public methods
public:
	ContainerManager(
		WeakPtr<gfx::ppu::PPUController> const& ppuController,
		WeakPtr<FontRegistry const> const& fontRegistry );
	~ContainerManager();

	FocusManager const& GetFocusManager() const;
	FocusManager& GetMutableFocusManager();

	void CreateRootContainer();
	void RecalcRootContainer();

	// Destroys the entire UI, use with caution
	void RecreateRootContainer();

	ContainerID GetContainerID( rftl::string_view label ) const;

	Container const& GetContainer( ContainerID containerID ) const;
	Container const& GetContainer( rftl::string_view label ) const;

	WeakPtr<Controller> GetMutableController( ContainerID containerID );
	WeakPtr<Controller> GetMutableController( rftl::string_view label );

	template<typename T>
	WeakPtr<T> GetMutableControllerAs( ContainerID containerID );
	template<typename T>
	WeakPtr<T> GetMutableControllerAs( rftl::string_view label );

	void AssignLabel( ContainerID containerID, rftl::string_view label );

	template<typename T>
	WeakPtr<T> AssignStrongController( ContainerID containerID, CreationPayload<T>&& controller );
	template<typename T>
	WeakPtr<T> AssignStrongController( ContainerID containerID, UniquePtr<T>&& controller );
	template<typename T>
	WeakPtr<T> AssignStrongController( Container& container, CreationPayload<T>&& controller );
	template<typename T>
	WeakPtr<T> AssignStrongController( Container& container, UniquePtr<T>&& controller );

	void SetRootRenderDepth( gfx::ppu::DepthLayer depth );
	gfx::ppu::DepthLayer GetRecommendedRenderDepth( ContainerID containerID ) const;
	gfx::ppu::DepthLayer GetRecommendedRenderDepth( Container const& container ) const;
	void AdjustRecommendedRenderDepth( ContainerID containerID, gfx::ppu::DepthLayer offset );
	void ResetRecommendedRenderDepth( ContainerID containerID );

	void SetRootAABBReduction( gfx::ppu::CoordElem delta );
	void SetDebugAABBReduction( gfx::ppu::CoordElem delta );

	void RequestHardRecalc( ContainerID containerID );

	void ProcessRecalcs();

	void Render() const;

	void DebugRender( bool uzeZlayers, bool includeAnchors, bool includeHidden ) const;


	//
	// Private methods
private:
	gfx::ppu::PPUController& GetRenderer() const;
	FontRegistry const& GetFontRegistry() const;

	Container& GetMutableRootContainer();

	ContainerID CreateChildContainer(
		Container& parentContainer,
		AnchorID leftConstraint,
		AnchorID rightConstraint,
		AnchorID topConstraint,
		AnchorID bottomConstraint );

	Container& GetMutableContainer( ContainerID containerID );

	void NotifyZoomFactorChange();

	bool HasValidConstraints( Container const& container ) const;

	bool IsHiddenFromRoot( Container const& container ) const;

	void RecalcRootContainer( bool force );

	void RecalcContainer( Container& container, bool force );
	void DestroyContainer( ContainerID containerID );

	AnchorID CreateAnchor( Container& container );
	void MoveAnchor( AnchorID anchorID, gfx::ppu::Coord pos );
	void DestroyAnchor( AnchorID anchorID );

	WeakPtr<Controller> AssignStrongControllerInternal( Container& container, UniquePtr<Controller>&& controller );

	void MarkForSoftRecalc( AnchorID anchorID );
	void MarkForHardRecalc( AnchorID anchorID );

	void ProcessDestruction( ContainerIDSet&& seedContainers, AnchorIDSet&& seedAnchors );


	//
	// Private data
private:
	UniquePtr<FocusManager> mFocusManager;
	WeakPtr<gfx::ppu::PPUController> mGraphics;
	WeakPtr<FontRegistry const> mFontRegistry;

	ContainerStorage mContainers;
	AnchorStorage mAnchors;

	ContainerID mLastContainerID = kInvalidContainerID;
	AnchorID mLastAnchorID = kInvalidAnchorID;

	LabelToContainerID mLabelsToContainerIDs;

	AnchorIDSet mSoftRecalcsNeeded;
	AnchorIDSet mHardRecalcsNeeded;

	// TODO: Use actual value
	size_t mMaxDepthSeen = 30;

	gfx::ppu::DepthLayer mRootRenderDepth = 0;

	bool mIsDestroyingContainers = false;

	gfx::ppu::CoordElem mRootAABBReduction = 0;
	gfx::ppu::CoordElem mDebugAABBReduction = 0;

	gfx::ppu::ZoomFactor mMostRecentZoomFactor = gfx::ppu::kInvalidZoomFactor;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "ContainerManager.inl"

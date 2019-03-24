#include "stdafx.h"
#include "ContainerManager.h"

#include "GameUI/Container.h"
#include "GameUI/Anchor.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusManager.h"

#include "PPU/PPUController.h"

#include "core/meta/ScopedCleanup.h"
#include "core/ptr/default_creator.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

ContainerManager::ContainerManager(
	WeakPtr<gfx::PPUController> const& ppuController,
	WeakPtr<FontRegistry const> const& fontRegistry )
	: mFocusManager( DefaultCreator<FocusManager>::Create() )
	, mGraphics( ppuController )
	, mFontRegistry( fontRegistry )
{
	//
}



ContainerManager::~ContainerManager()
{
	//
}



FocusManager const& ContainerManager::GetFocusManager() const
{
	return *mFocusManager;
}



FocusManager& ContainerManager::GetMutableFocusManager()
{
	return *mFocusManager;
}



void ContainerManager::CreateRootContainer()
{
	RF_ASSERT( mContainers.count( kRootContainerID ) == 0 );

	RF_ASSERT( mLastContainerID == kInvalidContainerID );
	static_assert( kRootContainerID == kInvalidContainerID + 1, "Unexpected id scheme" );
	mLastContainerID = kRootContainerID;

	Container& root = mContainers[kRootContainerID];
	root.mContainerID = kRootContainerID;
	RecalcRootContainer( true );
}



void ContainerManager::RecalcRootContainer()
{
	RecalcRootContainer( false );
}



void ContainerManager::RecreateRootContainer()
{
	ProcessDestruction( { kRootContainerID }, {} );

	Container& root = mContainers[kRootContainerID];
	root.mContainerID = kRootContainerID;
	RecalcRootContainer( true );

	RF_ASSERT( mContainers.size() == 1 );
	RF_ASSERT( mAnchors.empty() );
	#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	{
		for( LabelToContainerID::value_type const& label : mLabelsToContainerIDs )
		{
			RF_ASSERT( label.second == kRootContainerID );
		}
	}
	#endif
}



ContainerID ContainerManager::GetContainerID( char const* label ) const
{
	return mLabelsToContainerIDs.at( label );
}



ContainerID ContainerManager::GetContainerID( rftl::string const& label ) const
{
	return mLabelsToContainerIDs.at( label );
}



Container const& ContainerManager::GetContainer( ContainerID containerID ) const
{
	return mContainers.at( containerID );
}



Container const& ContainerManager::GetContainer( char const* label ) const
{
	return GetContainer( GetContainerID( label ) );
}



Container const& ContainerManager::GetContainer( rftl::string const& label ) const
{
	return GetContainer( GetContainerID( label ) );
}



WeakPtr<Controller> ContainerManager::GetMutableController( ContainerID containerID )
{
	return GetContainer( containerID ).mWeakUIController;
}



WeakPtr<Controller> ContainerManager::GetMutableController( char const* label )
{
	return GetContainer( label ).mWeakUIController;
}



WeakPtr<Controller> ContainerManager::GetMutableController( rftl::string const& label )
{
	return GetContainer( label ).mWeakUIController;
}



void ContainerManager::AssignLabel( ContainerID containerID, char const* label )
{
	RF_ASSERT( mLabelsToContainerIDs.count( label ) == 0 );
	mLabelsToContainerIDs[label] = containerID;
}



void ContainerManager::SetRootRenderDepth( gfx::PPUDepthLayer depth )
{
	mRootRenderDepth = depth;
}



gfx::PPUDepthLayer ContainerManager::GetRecommendedRenderDepth( Container const& container ) const
{
	static_assert( gfx::kNearestLayer < gfx::kFarthestLayer, "Expected negative to be closer" );

	// Walk back up tree to root
	gfx::PPUDepthLayer retVal = mRootRenderDepth;
	ContainerID id = container.mParentContainerID;
	while( id != kInvalidContainerID )
	{
		Container const& currentContainer = mContainers.at( id );
		id = currentContainer.mParentContainerID;
		RF_ASSERT_MSG( retVal > gfx::kNearestLayer, "UI stack too deep" );
		retVal--;
	}
	return retVal;
}



void ContainerManager::SetRootAABBReduction( gfx::PPUCoordElem delta )
{
	mRootAABBReduction = delta;
}



void ContainerManager::SetDebugAABBReduction( gfx::PPUCoordElem delta )
{
	mDebugAABBReduction = delta;
}



void ContainerManager::ProcessRecalcs()
{
	// Controllers might be defective, and result in an infinite chain, so
	//  gaurd against infinite loops by assuming that a reasonable chain
	//  can't go beyond the maximum depth we've ever seen
	// NOTE: Caching the max depth before starting, since controllers could
	//  create children, which would trivially defeat detection of an
	//  infinite loop here
	size_t const maxPasses = mMaxDepthSeen;
	for( size_t pass = 0; pass < maxPasses; pass++ )
	{
		if( mRecalcsNeeded.empty() )
		{
			// No further work
			return;
		}

		AnchorIDSet const recalcsInProgress = rftl::move( mRecalcsNeeded );
		RF_ASSERT( mRecalcsNeeded.empty() );

		// For each anchor that needs updating...
		for( AnchorID const& anchorID : recalcsInProgress )
		{
			RF_ASSERT( anchorID != kInvalidAnchorID );

			// For each container that could be affected...
			for( ContainerStorage::value_type& containerEntry : mContainers )
			{
				Container& container = containerEntry.second;

				// If it's been affected...
				if( container.IsConstrainedBy( anchorID ) )
				{
					// Recalculate it
					RecalcContainer( container, false );
				}
			}
		}
	}

	// TODO: Log instead
	RF_DBGFAIL_MSG( "INFINITE LOOP" );
}



void ContainerManager::Render() const
{
	// Visit all child containers, starting with root
	ContainerIDSet containersToVisit;
	containersToVisit.emplace( kRootContainerID );
	while( containersToVisit.empty() == false )
	{
		// Pop from unvisited list
		ContainerID const currentID = *containersToVisit.begin();
		containersToVisit.erase( containersToVisit.begin() );
		Container const& container = mContainers.at( currentID );

		// All constraints should've been figure out by the time we render
		RF_ASSERT( HasValidConstraints( container ) );

		// Render
		bool shouldRenderChildren = true;
		UIConstContext const context( *this );
		container.OnRender( context, shouldRenderChildren );

		if( shouldRenderChildren )
		{
			// Add all child containers to unvisited list
			for( ContainerID const& childID : container.mChildContainerIDs )
			{
				RF_ASSERT( containersToVisit.count( childID ) == 0 );
				containersToVisit.emplace( childID );
			}
		}
	}
}



void ContainerManager::DebugRender() const
{
	float const minLum = math::Color3f::kGray25.r;
	float const maxLum = math::Color3f::kGray50.r;
	constexpr gfx::PPUCoordElem kAnchorRadius = 3;

	for( ContainerStorage::value_type const& containerEntry : mContainers )
	{
		ContainerID const id = containerEntry.first;
		Container const& container = containerEntry.second;
		math::Color3f const color = math::Color3f::RandomFromHash( id ).ClampLuminance( minLum, maxLum );
		mGraphics->DebugDrawAABB( container.mAABB, 3, color );
	}

	for( AnchorStorage::value_type const& anchorEntry : mAnchors )
	{
		Anchor const& anchor = anchorEntry.second;
		ContainerID const parentID = anchor.mParentContainerID;
		gfx::PPUCoord const& pos = anchor.mPos;
		math::Color3f const color = math::Color3f::RandomFromHash( parentID ).ClampLuminance( minLum, maxLum );
		mGraphics->DebugDrawLine( pos + gfx::PPUCoord{ -kAnchorRadius, -kAnchorRadius }, pos + gfx::PPUCoord{ kAnchorRadius, kAnchorRadius }, 2, color );
		mGraphics->DebugDrawLine( pos + gfx::PPUCoord{ -kAnchorRadius, kAnchorRadius }, pos + gfx::PPUCoord{ kAnchorRadius, -kAnchorRadius }, 2, color );
	}
}

///////////////////////////////////////////////////////////////////////////////

gfx::PPUController& ContainerManager::GetRenderer() const
{
	gfx::PPUController* const graphics = mGraphics;
	RF_ASSERT( graphics != nullptr );
	return *graphics;
}



FontRegistry const& ContainerManager::GetFontRegistry() const
{
	FontRegistry const* const fontRegistry = mFontRegistry;
	RF_ASSERT( fontRegistry != nullptr );
	return *fontRegistry;
}



Container& ContainerManager::GetMutableRootContainer()
{
	return GetMutableContainer( kRootContainerID );
}



ContainerID ContainerManager::CreateChildContainer( Container& parentContainer, AnchorID leftConstraint, AnchorID rightConstraint, AnchorID topConstraint, AnchorID bottomConstraint )
{
	ContainerID const parentContainerID = parentContainer.mContainerID;
	RF_ASSERT( parentContainerID != kInvalidContainerID );

	mLastContainerID++;
	ContainerID const childContainerID = mLastContainerID;

	parentContainer.mChildContainerIDs.emplace_back( childContainerID );

	RF_ASSERT( mContainers.count( childContainerID ) == 0 );
	Container& childContainer = mContainers[childContainerID];
	childContainer.mContainerID = childContainerID;
	childContainer.mParentContainerID = parentContainerID;
	childContainer.mLeftConstraint = leftConstraint;
	childContainer.mRightConstraint = rightConstraint;
	childContainer.mTopConstraint = topConstraint;
	childContainer.mBottomConstraint = bottomConstraint;

	// TODO: Update max depth ever seen
	// TODO: Warn if getting crazy

	RecalcContainer( childContainer, true );

	return childContainerID;
}



Container& ContainerManager::GetMutableContainer( ContainerID containerID )
{
	return mContainers.at( containerID );
}



bool ContainerManager::HasValidConstraints( Container const& container ) const
{
	if( container.mContainerID == kRootContainerID )
	{
		// Root is always properly constrained, by definition
		return true;
	}

	Anchor const& leftConstraint = mAnchors.at( container.mLeftConstraint );
	Anchor const& topConstraint = mAnchors.at( container.mTopConstraint );
	Anchor const& rightConstraint = mAnchors.at( container.mRightConstraint );
	Anchor const& bottomConstraint = mAnchors.at( container.mBottomConstraint );

	bool const allConstraintsValid =
		leftConstraint.HasValidPosition() &&
		topConstraint.HasValidPosition() &&
		rightConstraint.HasValidPosition() &&
		bottomConstraint.HasValidPosition();

	if( allConstraintsValid == false )
	{
		return false;
	}

	// It's 'valid', but is it correct?
	RF_ASSERT( leftConstraint.mPos.x < rightConstraint.mPos.x );
	RF_ASSERT( topConstraint.mPos.y < bottomConstraint.mPos.y );

	return true;
}



void ContainerManager::RecalcRootContainer( bool force )
{
	Container& root = GetMutableRootContainer();

	Container::AABB4 newAABB;
	newAABB.mTopLeft = { mRootAABBReduction, mRootAABBReduction };
	newAABB.mBottomRight.x = mGraphics->GetWidth() - mRootAABBReduction;
	newAABB.mBottomRight.y = mGraphics->GetHeight() - mRootAABBReduction;

	if( ( newAABB != root.mAABB ) || force )
	{
		root.mAABB = newAABB;
		UIContext context( *this );
		root.OnAABBRecalc( context );
	}
}



void ContainerManager::RecalcContainer( Container& container, bool force )
{
	if( container.mContainerID == kRootContainerID )
	{
		// Root container has special handling logic since it doesn't have
		//  normal constraints
		RecalcRootContainer( force );
		return;
	}

	if( HasValidConstraints( container ) == false )
	{
		// Not able to recalc yet. This may be due to a chain of containers
		//  being created still on the stack, which will need to wait for a
		//  recalc pass to calculate the constraints in order
		return;
	}

	Anchor const& leftConstraint = mAnchors.at( container.mLeftConstraint );
	Anchor const& topConstraint = mAnchors.at( container.mTopConstraint );
	Anchor const& rightConstraint = mAnchors.at( container.mRightConstraint );
	Anchor const& bottomConstraint = mAnchors.at( container.mBottomConstraint );

	Container::AABB4 newAABB;
	newAABB.mTopLeft.x = leftConstraint.mPos.x + mDebugAABBReduction;
	newAABB.mTopLeft.y = topConstraint.mPos.y + mDebugAABBReduction;
	newAABB.mBottomRight.x = rightConstraint.mPos.x - mDebugAABBReduction;
	newAABB.mBottomRight.y = bottomConstraint.mPos.y - mDebugAABBReduction;

	RF_ASSERT( newAABB.Width() > 0 );
	RF_ASSERT( newAABB.Height() > 0 );

	if( ( newAABB != container.mAABB ) || force )
	{
		container.mAABB = newAABB;
		UIContext context( *this );
		container.OnAABBRecalc( context );
	}
}



void ContainerManager::DestroyContainer( ContainerID containerID )
{
	ProcessDestruction( { containerID }, {} );
}



AnchorID ContainerManager::CreateAnchor( Container& container )
{
	ContainerID const containerID = container.mContainerID;
	RF_ASSERT( containerID != kInvalidContainerID );

	mLastAnchorID++;
	AnchorID const anchorID = mLastAnchorID;

	container.mAnchorIDs.emplace_back( anchorID );

	RF_ASSERT( mAnchors.count( anchorID ) == 0 );
	Anchor& anchor = mAnchors[anchorID];
	anchor.mAnchorID = anchorID;
	anchor.mParentContainerID = containerID;
	anchor.InvalidatePosition();

	return anchorID;
}



void ContainerManager::MoveAnchor( AnchorID anchorID, gfx::PPUCoord pos )
{
	RF_ASSERT( anchorID != kInvalidAnchorID );
	Anchor& anchor = mAnchors.at( anchorID );
	anchor.mPos = pos;

	RF_ASSERT( anchor.mPos.x >= 0 );
	RF_ASSERT( anchor.mPos.y >= 0 );
	RF_ASSERT( anchor.mPos.x <= mContainers.at( kRootContainerID ).mAABB.Right() );
	RF_ASSERT( anchor.mPos.y <= mContainers.at( kRootContainerID ).mAABB.Bottom() );

	MarkForRecalc( anchorID );
}



void ContainerManager::DestroyAnchor( AnchorID anchorID )
{
	ProcessDestruction( {}, { anchorID } );
}



WeakPtr<Controller> ContainerManager::AssignStrongControllerInternal( Container& container, UniquePtr<Controller>&& controller )
{
	RF_ASSERT( controller != nullptr );
	RF_ASSERT( container.mStrongUIController == nullptr );
	RF_ASSERT( container.mWeakUIController == nullptr );
	container.mStrongUIController = rftl::move( controller );
	container.mWeakUIController = container.mStrongUIController;

	UIContext context( *this );
	container.OnAssign( context );
	RecalcContainer( container, true );

	RF_ASSERT( container.mWeakUIController != nullptr );
	return container.mWeakUIController;
}



void ContainerManager::MarkForRecalc( AnchorID anchorID )
{
	mRecalcsNeeded.emplace( anchorID );
}



void ContainerManager::ProcessDestruction( ContainerIDSet&& seedContainers, AnchorIDSet&& seedAnchors )
{
	// Re-entrant destruction not allowed
	RF_ASSERT_MSG( mIsDestroyingContainers == false, "Re-entrant destruction" );
	mIsDestroyingContainers = true;
	auto const onScopeEnd = RF::OnScopeEnd( [this]() {
		mIsDestroyingContainers = false;
	} );

	ContainerIDSet containersToDestroy = rftl::move( seedContainers );
	AnchorIDSet anchorsToDestroy = rftl::move( seedAnchors );

	while( true )
	{
		size_t const initialSize = containersToDestroy.size() + anchorsToDestroy.size();

		// Visit all child containers, starting with seed containers
		ContainerIDSet containersToVisit = containersToDestroy;
		while( containersToVisit.empty() == false )
		{
			// Pop from unvisited list
			ContainerID const currentID = *containersToVisit.begin();
			containersToVisit.erase( containersToVisit.begin() );
			Container const& container = mContainers.at( currentID );

			// Mark container for deletion
			containersToDestroy.emplace( currentID );

			// Mark all anchors for deletion
			for( AnchorID const& anchorID : container.mAnchorIDs )
			{
				anchorsToDestroy.emplace( anchorID );
			}

			// Add all child containers to unvisited list
			for( ContainerID const& childID : container.mChildContainerIDs )
			{
				containersToVisit.emplace( childID );
			}
		}

		// For each anchor that needs updating...
		AnchorIDSet anchorsToVisit = anchorsToDestroy;
		for( AnchorID const& anchorID : anchorsToVisit )
		{
			RF_ASSERT( anchorID != kInvalidAnchorID );

			// For each container that could be affected...
			for( ContainerStorage::value_type& containerEntry : mContainers )
			{
				ContainerID const& containerID = containerEntry.first;
				Container const& container = containerEntry.second;
				RF_ASSERT( container.mContainerID == containerID );

				// If it's been affected...
				if( container.IsConstrainedBy( anchorID ) )
				{
					// Destroy it
					containersToDestroy.emplace( containerID );
				}
			}
		}

		// Did the pass find more to destroy?
		size_t const resultingSize = containersToDestroy.size() + anchorsToDestroy.size();
		if( resultingSize != initialSize )
		{
			// Yes? Run another pass
			RF_ASSERT( resultingSize > initialSize );
			continue;
		}
		else
		{
			// No? Then no more passes needed
			break;
		}
	}

	// Erase containers
	for( ContainerID const& currentID : containersToDestroy )
	{
		// Final access before erase
		ContainerID parentContainerID = kInvalidContainerID;
		{
			Container const& container = mContainers.at( currentID );
			parentContainerID = container.mParentContainerID;

			// Notify container
			// NOTE: Const operation, containers are not allowed to interfere
			//  with the destruction process
			static_assert( rftl::is_const<rftl::remove_reference<decltype( container )>::type>::value, "Not const" );
			UIContext context( *this );
			container.OnImminentDestruction( context );
		}

		// Erase
		RF_ASSERT( mContainers.count( currentID ) == 1 );
		mContainers.erase( currentID );

		// Remove from parent
		// NOTE: Parent may have just recently been destroyed
		if( parentContainerID != kInvalidContainerID )
		{
			ContainerStorage::iterator findIter = mContainers.find( parentContainerID );
			if( findIter != mContainers.end() )
			{
				Container& parentContainer = findIter->second;
				ContainerIDList& children = parentContainer.mChildContainerIDs;
				ContainerIDList::iterator toErase = children.end();
				for( ContainerIDList::iterator iter = children.begin(); iter != children.end(); iter++ )
				{
					if( *iter == currentID )
					{
						RF_ASSERT( toErase == children.end() );
						toErase = iter;
					}
				}
				RF_ASSERT( toErase != children.end() );
				children.erase( toErase );
			}
		}

		// Remove any labels
		for( LabelToContainerID::iterator iter = mLabelsToContainerIDs.begin(); iter != mLabelsToContainerIDs.end(); iter++ )
		{
			if( iter->second == currentID )
			{
				iter = mLabelsToContainerIDs.erase( iter );
				if( iter == mLabelsToContainerIDs.end() )
				{
					// Must exit here, otherwise iter++ will happen before !=
					//  check runs
					break;
				}
			}
		}
	}

	// Erase anchors
	for( AnchorID currentID : anchorsToDestroy )
	{
		RF_ASSERT( mAnchors.count( currentID ) == 1 );

		// Find the parent container if it hasn't been destroyed already
		ContainerID parentContainerID;
		{
			Anchor const& anchor = mAnchors.at( currentID );
			parentContainerID = anchor.mParentContainerID;
			RF_ASSERT( parentContainerID != kInvalidContainerID );
		}
		ContainerStorage::iterator const containerIter = mContainers.find( parentContainerID );
		if( containerIter == mContainers.end() )
		{
			// Already destroyed
		}
		else
		{
			// Remove anchor from container
			Container& container = containerIter->second;
			bool destroyedAnchor = false;
			for( AnchorIDList::iterator iter = container.mAnchorIDs.begin(); iter != container.mAnchorIDs.end(); iter++ )
			{
				if( *iter == currentID )
				{
					container.mAnchorIDs.erase( iter );
					destroyedAnchor = true;
					break;
				}
			}
			RF_ASSERT( destroyedAnchor );
		}

		// Erase
		mAnchors.erase( currentID );
	}

	// Clear out orphaned focus data
	mFocusManager->GetMutableFocusTree().TrimDeadLinks();
}

///////////////////////////////////////////////////////////////////////////////
}}

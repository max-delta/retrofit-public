#include "stdafx.h"
#include "ContainerManager.h"

#include "GameUI/Container.h"
#include "GameUI/Anchor.h"

#include "PPU/PPUController.h"

#include "core/meta/ScopedCleanup.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

ContainerManager::ContainerManager( WeakPtr<gfx::PPUController> const& ppuController )
{
	mGraphics = ppuController;
}



ContainerManager::~ContainerManager()
{
	//
}



void ContainerManager::CreateRootContainer()
{
	RF_ASSERT( mContainers.count( kRootContainerID ) == 0 );

	RF_ASSERT( mLastContainerID == kInvalidContainerID );
	static_assert( kRootContainerID == kInvalidContainerID + 1, "Unexpected id scheme" );
	mLastContainerID = kRootContainerID;

	Container& root = mContainers[kRootContainerID];
	root.mContainerID = kRootContainerID;
	RecalcRootContainer();
}



void ContainerManager::RecalcRootContainer()
{
	Container& root = GetMutableRootContainer();

	// HACK: Slightly smaller than canvas, for testing
	constexpr gfx::PPUCoordElem kDelta = gfx::kTileSize / 4;

	Container::AABB4 newAABB;
	newAABB.mTopLeft = { kDelta, kDelta };
	newAABB.mBottomRight.x = mGraphics->GetWidth() - kDelta;
	newAABB.mBottomRight.y = mGraphics->GetHeight() - kDelta;

	if( newAABB != root.mAABB )
	{
		root.mAABB = newAABB;
		root.OnAABBRecalc( *this );
	}
}



Container const& ContainerManager::GetContainer( ContainerID containerID ) const
{
	return mContainers.at( containerID );
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
					RecalcContainer( container );
				}
			}
		}
	}

	// TODO: Log instead
	RF_DBGFAIL_MSG( "INFINITE LOOP" );
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

	RecalcContainer( childContainer );

	return childContainerID;
}



Container& ContainerManager::GetMutableContainer( ContainerID containerID )
{
	return mContainers.at( containerID );
}



void ContainerManager::RecalcContainer( Container& container )
{
	// HACK: Slightly smaller, for testing
	constexpr gfx::PPUCoordElem kDelta = gfx::kTileSize / 4;

	Container::AABB4 newAABB;
	newAABB.mTopLeft.x = mAnchors.at( container.mLeftConstraint ).mPos.x + kDelta;
	newAABB.mTopLeft.y = mAnchors.at( container.mTopConstraint ).mPos.y + kDelta;
	newAABB.mBottomRight.x = mAnchors.at( container.mRightConstraint ).mPos.x - kDelta;
	newAABB.mBottomRight.y = mAnchors.at( container.mBottomConstraint ).mPos.y - kDelta;

	if( newAABB != container.mAABB )
	{
		container.mAABB = newAABB;
		container.OnAABBRecalc( *this );
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

	return anchorID;
}



void ContainerManager::MoveAnchor( AnchorID anchorID, gfx::PPUCoord pos )
{
	RF_ASSERT( anchorID != kInvalidAnchorID );
	Anchor& anchor = mAnchors.at( anchorID );
	anchor.mPos = pos;

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

	container.OnAssign( *this );

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
		// Notify container
		// NOTE: Const operation, containers are not allowed to interfere
		//  with the destruction process
		{
			Container const& container = mContainers.at( currentID );
			static_assert( rftl::is_const<rftl::remove_reference<decltype( container )>::type>::value, "Not const" );
			container.OnImminentDestruction( *this );
		}

		// Erase
		RF_ASSERT( mContainers.count( currentID ) == 1 );
		mContainers.erase( currentID );
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
}

///////////////////////////////////////////////////////////////////////////////
}}

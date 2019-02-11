#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"

#include "core_math/AABB4.h"
#include "core_math/Lerp.h"
#include "core/ptr/default_creator.h"
#include "core/meta/ScopedCleanup.h"

#include "rftl/unordered_map"
#include "rftl/unordered_set"
#include "rftl/array"

namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// Container
// |- Container ID
// |- Parent container ID
// |- 4x Parent anchor points (Left/right/top/bottom constraint)
// |   L- Parent anchor point ID
// |- Last calc AABB from parent anchor points
// |- Nx Child containers
// |   L- Container ID
// |- Nx Child anchor points
// |   L- Anchor point ID
// L- UI controller
//     L- UniquePtr

using ContainerID = uint64_t;
static constexpr ContainerID kInvalidContainerID = 0;
static constexpr ContainerID kRootContainerID = 1;
using ContainerIDList = rftl::vector<ContainerID>;
using ContainerIDSet = rftl::unordered_set<ContainerID>;
using AnchorID = uint64_t;
using AnchorIDList = rftl::vector<AnchorID>;
using AnchorIDSet = rftl::unordered_set<AnchorID>;
static constexpr AnchorID kInvalidAnchorID = 0;

class UIController;
struct Container;
struct Anchor;
class ContainerManager;

class UIController
{
public:
	virtual ~UIController() = default;

	virtual void OnAssign( ContainerManager& manager, Container& container ) {}
	virtual void OnAABBRecalc( ContainerManager& manager, Container& container ) {}

	// NOTE: Do NOT destroy child containers or anchors, this will happen
	//  automatically, and attempting to do so here may cause re-entrancy
	//  violations and undefined behavior
	// NOTE: Container is const to help enforce this
	virtual void OnImminentDestruction( ContainerManager& manager, Container const& container ) {}
};

struct Container
{
	RF_NO_COPY( Container );

	using AABB4 = math::AABB4<gfx::PPUCoordElem>;

	Container() = default;

	// Containers exist in a hierarchy
	ContainerID mContainerID = kInvalidContainerID;
	ContainerID mParentContainerID = kInvalidContainerID;
	ContainerIDList mChildContainerIDs = {};

	// Containers are constrained by anchor points
	// NOTE: Not necessarily to those of their immediate parent, which further
	//  means that a child AABB may be larger than the AABB of its immediate
	//  parent
	AnchorID mLeftConstraint = kInvalidAnchorID;
	AnchorID mRightConstraint = kInvalidAnchorID;
	AnchorID mTopConstraint = kInvalidAnchorID;
	AnchorID mBottomConstraint = kInvalidAnchorID;
	AABB4 mAABB;

	// Containers may own anchor points, which descendants may constrain
	//  themselves to
	AnchorIDList mAnchorIDs;

	// Containers may have a controller that runs their logic
	// NOTE: Possible to only have a weak reference, such as when sharing
	//  controllers that are designed to manage multiple containers
	UniquePtr<UIController> mStrongUIController;
	WeakPtr<UIController> mWeakUIController;

	bool IsConstrainedBy( AnchorID anchorID ) const
	{
		if(
			mLeftConstraint == anchorID ||
			mRightConstraint == anchorID ||
			mTopConstraint == anchorID ||
			mBottomConstraint == anchorID )
		{
			return true;
		}
		return false;
	}

	void OnAssign( ContainerManager& manager )
	{
		UIController* const controller = mWeakUIController;
		RF_ASSERT( controller != nullptr );
		controller->OnAssign( manager, *this );
		controller->OnAABBRecalc( manager, *this );
	}

	void OnAABBRecalc( ContainerManager& manager )
	{
		UIController* const controller = mWeakUIController;
		if( controller != nullptr )
		{
			controller->OnAABBRecalc( manager, *this );
		}
	}

	void OnImminentDestruction( ContainerManager& manager ) const
	{
		UIController* const controller = mWeakUIController;
		if( controller != nullptr )
		{
			controller->OnImminentDestruction( manager, *this );
		}
	}
};

struct Anchor
{
	RF_NO_COPY( Anchor );

	Anchor() = default;

	// Anchors have no direct relationship to each other
	AnchorID mAnchorID = kInvalidAnchorID;

	// Anchors are owned by containers
	ContainerID mParentContainerID = kInvalidContainerID;

	// Anchors are dimensionless points
	gfx::PPUCoord mPos;
};

// + := done

//+Start with a special root container
//+ Root container has invalid parent anchor points
//+ Root container gets AABB from canvas
//+Assign a UI controller of some kind to the root container
//+ Expect it to be some kind of non-rendering controller
//+ Expect it to create anchor points on the controller
//+ It may create child containers, but probably not

//+On anchor point creation
//+ Generate new ID
//+ Add ID to large table which indicates which container ID it is owned by
//+On anchor point destruction
//+ Remove anchor point ID from large table
//+ Add anchor point ID to invalid list
//+ Flag that an anchor invalidation pass needs to happen
//+On anchor invalidation pass
//+ Clear flag
//+ Move invalid anchor list into local copy
//+ Walk entire tree, and remove any child container that was parented to any invalid anchor
//+ Check flag, repeat if it was set again
//+On container creation
//+ Calculate AABB
//  Update max depth of tree ever seen
//+On container destruction
//+ Destroy all anchor points
//+ Recurse down, destroy all child containers and anchor points
//+On anchor point move
//+ Add anchor point to recalc list
//+ Flag that an anchor recalc pass needs to happen
//+On anchor recalc pass
//+ Clear flag
//+ Move recalc anchor list into local copy
//+ Walk entire tree
//+  Recalc any AABBs that are parented to a moved anchor
//+  Notify any controllers of a container that had an AABB recalc (they may move their anchors in response)
//+ Check flag, repeat if it was set again, assert and bail if repeats more than max depth ever seen (loop found)
// On message (render, input, etc)
//  Dispatch breadth-first to controllers
//   Allow controller to block children from receiving (filter out controllers that are reachable from blocker)

class ContainerManager
{
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


	void Construct( WeakPtr<gfx::PPUController> const& ppuController )
	{
		mGraphics = ppuController;
	}



	void CreateRootContainer()
	{
		RF_ASSERT( mContainers.count( kRootContainerID ) == 0 );

		RF_ASSERT( mLastContainerID == kInvalidContainerID );
		static_assert( kRootContainerID == kInvalidContainerID + 1, "Unexpected id scheme" );
		mLastContainerID = kRootContainerID;

		Container& root = mContainers[kRootContainerID];
		root.mContainerID = kRootContainerID;
		RecalcRootContainer();
	}



	Container& GetMutableRootContainer()
	{
		return GetMutableContainer( kRootContainerID );
	}



	void RecalcRootContainer()
	{
		Container& root = GetMutableRootContainer();
		// HACK: Slightly smaller than canvas, for testing
		constexpr gfx::PPUCoordElem kDelta = gfx::kTileSize / 4;
		root.mAABB.mTopLeft = { kDelta, kDelta };
		root.mAABB.mBottomRight.x = mGraphics->GetWidth() - kDelta;
		root.mAABB.mBottomRight.y = mGraphics->GetHeight() - kDelta;
		root.OnAABBRecalc( *this );
	}



	ContainerID CreateChildContainer(
		Container& parentContainer,
		AnchorID leftConstraint,
		AnchorID rightConstraint,
		AnchorID topConstraint,
		AnchorID bottomConstraint )
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



	Container const& GetContainer( ContainerID containerID ) const
	{
		return mContainers.at( containerID );
	}



	Container& GetMutableContainer( ContainerID containerID )
	{
		return mContainers.at( containerID );
	}



	void RecalcContainer( Container& container )
	{
		// HACK: Slightly smaller, for testing
		constexpr gfx::PPUCoordElem kDelta = gfx::kTileSize / 4;

		container.mAABB.mTopLeft.x = mAnchors.at( container.mLeftConstraint ).mPos.x + kDelta;
		container.mAABB.mTopLeft.y = mAnchors.at( container.mTopConstraint ).mPos.y + kDelta;
		container.mAABB.mBottomRight.x = mAnchors.at( container.mRightConstraint ).mPos.x - kDelta;
		container.mAABB.mBottomRight.y = mAnchors.at( container.mBottomConstraint ).mPos.y - kDelta;

		container.OnAABBRecalc( *this );
	}



	void DestroyContainer( ContainerID containerID )
	{
		ProcessDestruction( { containerID }, {} );
	}



	AnchorID CreateAnchor( Container& container )
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
	};



	void MoveAnchor( AnchorID anchorID, gfx::PPUCoord pos )
	{
		RF_ASSERT( anchorID != kInvalidAnchorID );
		Anchor& anchor = mAnchors.at( anchorID );
		anchor.mPos = pos;

		MarkForRecalc( anchorID );
	};



	void DestroyAnchor( AnchorID anchorID )
	{
		ProcessDestruction( {}, { anchorID } );
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
		UniquePtr<UIController> temp = rftl::move( controller );
		WeakPtr<UIController> const result = AssignStrongControllerInternal( container, rftl::move( temp ) );
		RF_ASSERT( retVal == result );
		return retVal;
	}



	WeakPtr<UIController> AssignStrongControllerInternal( Container& container, UniquePtr<UIController>&& controller )
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



	void DebugRender() const
	{
		math::Color3f const& min = math::Color3f::kGray25;
		math::Color3f const& max = math::Color3f::kGray50;
		constexpr gfx::PPUCoordElem kAnchorRadius = 3;

		for( ContainerStorage::value_type const& containerEntry : mContainers )
		{
			ContainerID const id = containerEntry.first;
			Container const& container = containerEntry.second;
			math::Color3f const color = math::Color3f::RandomFromHash( id ).Clamp( min, max );
			mGraphics->DebugDrawAABB( container.mAABB, 3, color );
		}

		for( AnchorStorage::value_type const& anchorEntry : mAnchors )
		{
			Anchor const& anchor = anchorEntry.second;
			ContainerID const parentID = anchor.mParentContainerID;
			gfx::PPUCoord const& pos = anchor.mPos;
			math::Color3f const color = math::Color3f::RandomFromHash( parentID ).Clamp( min, max );
			mGraphics->DebugDrawLine( pos + gfx::PPUCoord{ -kAnchorRadius, -kAnchorRadius }, pos + gfx::PPUCoord{ kAnchorRadius, kAnchorRadius }, 2, color );
			mGraphics->DebugDrawLine( pos + gfx::PPUCoord{ -kAnchorRadius, kAnchorRadius }, pos + gfx::PPUCoord{ kAnchorRadius, -kAnchorRadius }, 2, color );
		}
	}



	void MarkForRecalc( AnchorID anchorID )
	{
		mRecalcsNeeded.emplace( anchorID );
	}



	void ProcessRecalcs()
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



	void ProcessDestruction(ContainerIDSet && seedContainers, AnchorIDSet && seedAnchors)
	{
		// Re-entrant destruction not allowed
		RF_ASSERT_MSG( mIsDestroyingContainers == false, "Re-entrant destruction" );
		mIsDestroyingContainers = true;
		auto const onScopeEnd = RF::OnScopeEnd( [this]()
		{
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
};


namespace controller {

// 0 1 2
// 3 4 5
// 6 7 8
class NineSlicer final : public UIController
{
public:
	NineSlicer()
	{
		for( bool& b : mSliceEnabled )
		{
			b = true;
		}
	}

	NineSlicer( bool const (&sliceEnabled)[9] )
	{
		for( size_t i = 0; i < 9; i++ )
		{
			mSliceEnabled[i] = sliceEnabled[i];
		}
	}

	ContainerID GetChildContainerID( size_t sliceIndex ) const
	{
		return mContainers.at( sliceIndex );
	}

	void DestroyChildContainer( ContainerManager& manager, size_t sliceIndex )
	{
		ContainerID& id = mContainers.at( sliceIndex );
		if( id != kInvalidContainerID )
		{
			manager.DestroyContainer( id );
			id = kInvalidContainerID;
			mSliceEnabled[sliceIndex] = false;
		}
		else
		{
			RF_ASSERT( mSliceEnabled[sliceIndex] == false );
		}
	}

	virtual void OnAssign( ContainerManager& manager, Container& container ) override
	{
		m0 = manager.CreateAnchor( container );
		m33 = manager.CreateAnchor( container );
		m66 = manager.CreateAnchor( container );
		m100 = manager.CreateAnchor( container );

		for( size_t i = 0; i < 9; i++ )
		{
			if( mSliceEnabled[i] == false )
			{
				mContainers[i] = kInvalidContainerID;
				continue;
			}

			AnchorID top;
			AnchorID bottom;
			if( i < 3 )
			{
				top = m0;
				bottom = m33;
			}
			else if( i < 6 )
			{
				top = m33;
				bottom = m66;
			}
			else
			{
				top = m66;
				bottom = m100;
			}

			AnchorID left;
			AnchorID right;
			size_t const column = i % 3;
			if( column == 0 )
			{
				left = m0;
				right = m33;
			}
			else if( column == 1 )
			{
				left = m33;
				right = m66;
			}
			else
			{
				left = m66;
				right = m100;
			}

			mContainers[i] = manager.CreateChildContainer( container, left, right, top, bottom );
		}
	}

	virtual void OnAABBRecalc( ContainerManager& manager, Container& container ) override
	{
		Container::AABB4 const& aabb = container.mAABB;
		gfx::PPUCoordElem const x0 = aabb.Left();
		gfx::PPUCoordElem const x100 = aabb.Right();
		gfx::PPUCoordElem const x33 = math::Lerp( x0, x100, 1.f / 3.f );
		gfx::PPUCoordElem const x66 = math::Lerp( x0, x100, 2.f / 3.f );
		gfx::PPUCoordElem const y0 = aabb.Top();
		gfx::PPUCoordElem const y100 = aabb.Bottom();
		gfx::PPUCoordElem const y33 = math::Lerp( y0, y100, 1.f / 3.f );
		gfx::PPUCoordElem const y66 = math::Lerp( y0, y100, 2.f / 3.f );

		manager.MoveAnchor( m0, { x0, y0 } );
		manager.MoveAnchor( m33, { x33, y33 } );
		manager.MoveAnchor( m66, { x66, y66 } );
		manager.MoveAnchor( m100, { x100, y100 } );
	}

private:
	AnchorID m0 = kInvalidAnchorID;
	AnchorID m33 = kInvalidAnchorID;
	AnchorID m66 = kInvalidAnchorID;
	AnchorID m100 = kInvalidAnchorID;
	rftl::array<ContainerID, 9> mContainers = {};
	bool mSliceEnabled[9] = {};
};



class Passthrough final : public UIController
{
public:
	ContainerID GetChildContainerID()
	{
		return mContainerID;
	}

	virtual void OnAssign( ContainerManager& manager, Container& container ) override
	{
		mContainerID = manager.CreateChildContainer(
			container,
			container.mLeftConstraint,
			container.mRightConstraint,
			container.mTopConstraint,
			container.mBottomConstraint );
	}

private:
	ContainerID mContainerID = kInvalidContainerID;
};

}

ContainerManager tempUI;

void SetupStructures()
{
	tempUI.Construct( app::gGraphics );
	tempUI.CreateRootContainer();
	tempUI.RecalcRootContainer();
	constexpr bool kSlicesEnabled[9] = { false, false, true, false, false, false, true, false, true };
	WeakPtr<controller::NineSlicer> const nineSlicer =
		tempUI.AssignStrongController(
			tempUI.GetMutableRootContainer(),
			DefaultCreator<controller::NineSlicer>::Create(
				kSlicesEnabled ) );

	WeakPtr<controller::Passthrough> const passthrough8 =
		tempUI.AssignStrongController(
			tempUI.GetMutableContainer( nineSlicer->GetChildContainerID( 8 ) ),
			DefaultCreator<controller::Passthrough>::Create() );
	WeakPtr<controller::Passthrough> const passthrough8_1 =
		tempUI.AssignStrongController(
			tempUI.GetMutableContainer( passthrough8->GetChildContainerID() ),
			DefaultCreator<controller::Passthrough>::Create() );
	// TODO: Create a chain of containers, and then blow up a parent to watch
	//  them all recursively destroy themselves, and confirm they cleaned up
	//  properly without any dangling artifacts
	// TODO: Make a bunch of manager stuff private with friending, so only
	//  the UI controller base class can initiate structural changes
	tempUI.ProcessRecalcs();
	nineSlicer->DestroyChildContainer( tempUI, 8 );
}

void Render()
{
	tempUI.DebugRender();
}

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;
	SetupStructures();
}



void Run()
{
	using namespace details;
	Render();
}



void End()
{
	using namespace details;
}

///////////////////////////////////////////////////////////////////////////////
}}

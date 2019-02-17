#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/controllers/NineSlicer.h"
#include "GameUI/controllers/Passthrough.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"


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

UniquePtr<ui::ContainerManager> tempContainerManager;

void SetupStructures()
{
	tempContainerManager = DefaultCreator<ui::ContainerManager>::Create( app::gGraphics );
	ui::ContainerManager& tempUI = *tempContainerManager;

	tempUI.CreateRootContainer();

	// Slice the root canvas
	constexpr bool kSlicesEnabled[9] = { false, false, true, false, false, false, true, false, true };
	WeakPtr<ui::controller::NineSlicer> const nineSlicer =
		tempUI.AssignStrongController(
			tempUI.GetMutableRootContainer(),
			DefaultCreator<ui::controller::NineSlicer>::Create(
				kSlicesEnabled ) );

	// Create some passthoughs, and then blow them up from the head
	WeakPtr<ui::controller::Passthrough> const passthrough8 =
		tempUI.AssignStrongController(
			tempUI.GetMutableContainer( nineSlicer->GetChildContainerID( 8 ) ),
			DefaultCreator<ui::controller::Passthrough>::Create() );
	WeakPtr<ui::controller::Passthrough> const passthrough8_1 =
		tempUI.AssignStrongController(
			tempUI.GetMutableContainer( passthrough8->GetChildContainerID() ),
			DefaultCreator<ui::controller::Passthrough>::Create() );
	RF_ASSERT( passthrough8 != nullptr );
	RF_ASSERT( passthrough8_1 != nullptr );
	nineSlicer->DestroyChildContainer( tempUI, 8 );
	RF_ASSERT( passthrough8 == nullptr );
	RF_ASSERT( passthrough8_1 == nullptr );

	// Restore the container we blew up
	nineSlicer->CreateChildContainer( tempUI, 8 );

	// TODO: Make a bunch of manager stuff private with friending, so only
	//  the UI controller base class can initiate structural changes
}

void Render()
{
	ui::ContainerManager& tempUI = *tempContainerManager;
	tempUI.RecalcRootContainer();
	tempUI.ProcessRecalcs();
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

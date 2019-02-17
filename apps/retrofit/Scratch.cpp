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

// TODO: UI
//  On message (render, input, etc)
//   Dispatch breadth-first to controllers
//    Allow controller to block children from receiving (filter out controllers that are reachable from blocker)

static UniquePtr<ui::ContainerManager> tempContainerManager;

void SetupStructures()
{
	tempContainerManager = DefaultCreator<ui::ContainerManager>::Create( app::gGraphics );
	ui::ContainerManager& tempUI = *tempContainerManager;

	tempUI.CreateRootContainer();

	// Slice the root canvas
	constexpr bool kSlicesEnabled[9] = { false, false, true, false, false, false, true, false, true };
	WeakPtr<ui::controller::NineSlicer> const nineSlicer =
		tempUI.AssignStrongController(
			ui::kRootContainerID,
			DefaultCreator<ui::controller::NineSlicer>::Create(
				kSlicesEnabled ) );

	// Create some passthoughs, and then blow them up from the head
	WeakPtr<ui::controller::Passthrough> const passthrough8 =
		tempUI.AssignStrongController(
			nineSlicer->GetChildContainerID( 8 ),
			DefaultCreator<ui::controller::Passthrough>::Create() );
	WeakPtr<ui::controller::Passthrough> const passthrough8_1 =
		tempUI.AssignStrongController(
			passthrough8->GetChildContainerID(),
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

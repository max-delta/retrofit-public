#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/controllers/NineSlicer.h"
#include "GameUI/controllers/Passthrough.h"
#include "GameUI/controllers/TextLabel.h"

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
	tempUI.SetRootRenderDepth( gfx::kNearestLayer + 50 );

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

	// Add a text label
	WeakPtr<ui::controller::TextLabel> const textLabel8 =
		tempUI.AssignStrongController(
			nineSlicer->GetChildContainerID( 8 ),
			DefaultCreator<ui::controller::TextLabel>::Create() );
	textLabel8->SetFont( 2, 8 );
	textLabel8->SetText( "TextLabel8 test" );
}

void Render()
{
	ui::ContainerManager& tempUI = *tempContainerManager;
	tempUI.RecalcRootContainer();
	tempUI.ProcessRecalcs();
	tempUI.DebugRender();
	tempUI.Render();
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

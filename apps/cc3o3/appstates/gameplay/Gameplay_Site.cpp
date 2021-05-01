#include "stdafx.h"
#include "Gameplay_Site.h"

#include "cc3o3/appstates/InputHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameInput/GameController.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Site::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Site::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
}



void Gameplay_Site::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Site::OnTick( AppStateTickContext& context )
{
	app::gGraphics->DebugDrawText( gfx::ppu::Coord( 32, 32 ), "TODO: Site" );

	// Process menu actions
	rftl::vector<input::GameCommand> const menuCommands =
		InputHelpers::GetGameplayInputToProcess( InputHelpers::GetSinglePlayer(), input::layer::GameMenu );
	for( input::GameCommand const& menuCommand : menuCommands )
	{
		if( menuCommand.mType == input::command::game::UIActivateSelection )
		{
			// Enter battle
			context.mManager.RequestDeferredStateChange( id::Gameplay_Battle );
		}
		else if( menuCommand.mType == input::command::game::UICancelSelection )
		{
			// Exit site
			context.mManager.RequestDeferredStateChange( id::Gameplay_Overworld );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}

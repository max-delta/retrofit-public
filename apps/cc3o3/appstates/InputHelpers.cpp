#include "stdafx.h"
#include "InputHelpers.h"

#include "cc3o3/input/InputFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "Timing/FrameClock.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

rftl::vector<ui::FocusEventType> InputHelpers::GetMainMenuInputToProcess()
{
	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	input::GameController const& controller = *controllerManager.GetGameController( input::player::P1, input::layer::MainMenu );

	// Fetch commands that were entered for this current frame
	rftl::vector<input::GameCommand> commands;
	rftl::virtual_back_inserter_iterator<input::GameCommand, decltype( commands )> parser( commands );
	controller.GetGameCommandStream( parser, time::FrameClock::now() );

	// Convert to UI
	rftl::vector<ui::FocusEventType> retVal;
	for( input::GameCommand const& command : commands )
	{
		switch( command.mType )
		{
			case input::command::game::UINavigateUp:
				retVal.emplace_back( ui::focusevent::Command_NavigateUp );
				break;
			case input::command::game::UINavigateDown:
				retVal.emplace_back( ui::focusevent::Command_NavigateDown );
				break;
			case input::command::game::UINavigateLeft:
				retVal.emplace_back( ui::focusevent::Command_NavigateLeft );
				break;
			case input::command::game::UINavigateRight:
				retVal.emplace_back( ui::focusevent::Command_NavigateRight );
				break;
			case input::command::game::UINavigateToFirst:
				retVal.emplace_back( ui::focusevent::Command_NavigateToFirst );
				break;
			case input::command::game::UINavigateToLast:
				retVal.emplace_back( ui::focusevent::Command_NavigateToLast );
				break;
			case input::command::game::UIActivateSelection:
				retVal.emplace_back( ui::focusevent::Command_ActivateCurrentFocus );
				break;
			case input::command::game::UICancelSelection:
				retVal.emplace_back( ui::focusevent::Command_CancelCurrentFocus );
				break;
			default:
				RF_DBGFAIL_MSG(
					"Encountered a command that doesn't have a UI mapping."
					" This input layer shouldn't have a mapping that the UI"
					" can't handle." );
				break;
		}
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}}

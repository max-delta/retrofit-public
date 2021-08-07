#include "stdafx.h"
#include "InputHelpers.h"

#include "cc3o3/Common.h"
#include "cc3o3/input/HardcodedSetup.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"
#include "GameInput/RawController.h"
#include "GameSync/RollbackInputManager.h"

#include "Timing/FrameClock.h"
#include "Localization/PageMapper.h"

#include "core_unicode/StringConvert.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static input::PlayerID sSinglePlayer = input::kInvalidPlayerID;

}
///////////////////////////////////////////////////////////////////////////////

void InputHelpers::SetSinglePlayer( input::PlayerID player )
{
	RF_ASSERT( player != input::kInvalidPlayerID );
	RF_ASSERT( details::sSinglePlayer == input::kInvalidPlayerID );
	details::sSinglePlayer = player;
}



input::PlayerID InputHelpers::GetSinglePlayer()
{
	RF_ASSERT( details::sSinglePlayer != input::kInvalidPlayerID );
	return details::sSinglePlayer;
}



void InputHelpers::ClearSinglePlayer()
{
	RF_ASSERT( details::sSinglePlayer != input::kInvalidPlayerID );
	details::sSinglePlayer = input::kInvalidPlayerID;
}



void InputHelpers::MakeLocal( input::PlayerID player )
{
	RF_ASSERT( player != input::kInvalidPlayerID );
	input::HardcodedRollbackIdentifiers const identifiers( player );
	gRollbackInputManager->AddLocalStreams( { //
		identifiers.mRollbackGameMenusID,
		identifiers.mRollbackGamplayID } );
}



void InputHelpers::MakeRemote( input::PlayerID player )
{
	RF_ASSERT( player != input::kInvalidPlayerID );
	input::HardcodedRollbackIdentifiers const identifiers( player );
	gRollbackInputManager->RemoveLocalStreams( { //
		identifiers.mRollbackGameMenusID,
		identifiers.mRollbackGamplayID } );
}



bool InputHelpers::HasRemotePlayers()
{
	input::ControllerManager::PlayerIDs const players = app::gInputControllerManager->GetRegisteredPlayers();
	sync::RollbackInputManager::StreamIdentifiers const localStreams = gRollbackInputManager->GetLocalStreams();
	for( input::PlayerID const& player : players )
	{
		RF_ASSERT( player != input::kInvalidPlayerID );
		if( player == input::player::Global )
		{
			continue;
		}
		input::HardcodedRollbackIdentifiers const identifiers( player );
		bool const local =
			localStreams.count( identifiers.mRollbackGameMenusID ) ||
			localStreams.count( identifiers.mRollbackGameMenusID );
		if( local == false )
		{
			return true;
		}
	}
	return false;
}



rftl::vector<ui::FocusEventType> InputHelpers::GetMainMenuInputToProcess()
{
	rftl::vector<input::GameCommand> const commands = GetGameplayInputToProcess( input::player::Global, input::layer::MainMenu );
	return ConvertUICommandsToFocusEvents( commands );
}



rftl::vector<ui::FocusEventType> InputHelpers::GetGameMenuInputToProcess( input::PlayerID player )
{
	rftl::vector<input::GameCommand> const commands = GetGameplayInputToProcess( player, input::layer::GameMenu );
	return ConvertUICommandsToFocusEvents( commands );
}



rftl::vector<input::GameCommand> InputHelpers::GetGameplayInputToProcess( input::PlayerID player, input::LayerID layer )
{
	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	input::GameController const& controller = *controllerManager.GetGameController( player, layer );

	// Fetch commands that were entered for this current frame
	rftl::vector<input::GameCommand> commands;
	rftl::virtual_back_inserter_iterator<input::GameCommand, decltype( commands )> parser( commands );
	controller.GetGameCommandStream( parser, time::FrameClock::now(), time::FrameClock::now() );

	return commands;
}



rftl::string InputHelpers::GetMainMenuPageMappedTextBuffer()
{
	return GetMainMenuPageMappedTextBuffer( rftl::numeric_limits<size_t>::max() );
}



rftl::string InputHelpers::GetMainMenuPageMappedTextBuffer( size_t maxLen )
{
	return app::gPageMapper->MapTo8Bit(
		unicode::ConvertToUtf32(
			GetMainMenuUnicodeTextBuffer( maxLen ) ) );
}


rftl::u16string InputHelpers::GetMainMenuUnicodeTextBuffer()
{
	return GetMainMenuUnicodeTextBuffer( rftl::numeric_limits<size_t>::max() );
}



rftl::u16string InputHelpers::GetMainMenuUnicodeTextBuffer( size_t maxLen )
{
	rftl::u16string retVal;
	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	input::RawController const& controller = *controllerManager.GetTextProvider( input::player::Global );
	controller.GetTextStream( retVal, maxLen );
	return retVal;
}



void InputHelpers::ClearMainMenuTextBuffer()
{
	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	input::RawController& controller = *controllerManager.GetTextProvider( input::player::Global );
	controller.ClearTextStream();
}



rftl::vector<ui::FocusEventType> InputHelpers::ConvertUICommandsToFocusEvents( rftl::vector<input::GameCommand> const& commands )
{
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
			case input::command::game::UINavigateToPreviousGroup:
				retVal.emplace_back( ui::focusevent::Command_NavigateToPreviousGroup );
				break;
			case input::command::game::UINavigateToNextGroup:
				retVal.emplace_back( ui::focusevent::Command_NavigateToNextGroup );
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
			case input::command::game::UIAuxiliaryAction1:
				retVal.emplace_back( ui::focusevent::Command_AuxiliaryAction1 );
				break;
			case input::command::game::UIAuxiliaryAction2:
				retVal.emplace_back( ui::focusevent::Command_AuxiliaryAction2 );
				break;
			case input::command::game::UIMenuAction:
				retVal.emplace_back( ui::focusevent::Command_MenuAction );
				break;
			case input::command::game::UIPauseAction:
				retVal.emplace_back( ui::focusevent::Command_PauseAction );
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
}

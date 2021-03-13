#include "stdafx.h"
#include "HardcodedSetup.h"

#include "cc3o3/Common.h"
#include "cc3o3/time/TimeFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/RawInputController.h"
#include "GameInput/HotkeyController.h"
#include "GameSync/RollbackController.h"
#include "GameSync/RollbackInputManager.h"

#include "PlatformInput_win32/WndProcInputDevice.h"

#include "core_platform/shim/winuser_shim.h"
#include "core/ptr/default_creator.h"


namespace RF::cc::input {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static WeakPtr<input::RawInputController> sRawInputController;



UniquePtr<input::RollbackController> WrapWithRollback(
	WeakPtr<input::GameController> source,
	rollback::InputStreamIdentifier const& identifier )
{
	UniquePtr<input::RollbackController> rollbackController = DefaultCreator<input::RollbackController>::Create();
	rollbackController->SetSource( source );
	gRollbackInputManager->AddController( identifier, rollbackController );
	return rollbackController;
}



input::RawInputController::LogicalMapping HardcodedRawMapping()
{
	input::RawInputController::LogicalMapping retVal;
	{
		// WASD
		retVal['W'][input::DigitalPinState::Active] = command::raw::Up;
		retVal['W'][input::DigitalPinState::Inactive] = command::raw::UpStop;
		retVal['A'][input::DigitalPinState::Active] = command::raw::Left;
		retVal['A'][input::DigitalPinState::Inactive] = command::raw::LeftStop;
		retVal['S'][input::DigitalPinState::Active] = command::raw::Down;
		retVal['S'][input::DigitalPinState::Inactive] = command::raw::DownStop;
		retVal['D'][input::DigitalPinState::Active] = command::raw::Right;
		retVal['D'][input::DigitalPinState::Inactive] = command::raw::RightStop;

		retVal[shim::VK_OEM_4][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with arrows ('[')
		retVal[shim::VK_OEM_6][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with arrows (']')
		retVal[shim::VK_PRIOR][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with arrows
		retVal[shim::VK_NEXT][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with arrows
		retVal[shim::VK_HOME][input::DigitalPinState::Active] = command::raw::Home; // NOTE: Shared with arrows
		retVal[shim::VK_END][input::DigitalPinState::Active] = command::raw::End; // NOTE: Shared with arrows

		retVal[shim::VK_RETURN][input::DigitalPinState::Active] = command::raw::Affirmative;
		retVal['E'][input::DigitalPinState::Active] = command::raw::Affirmative;
		retVal[shim::VK_BACK][input::DigitalPinState::Active] = command::raw::Negative;
		retVal[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with arrows
		retVal[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary1; // NOTE: Shared with arrows
		retVal['X'][input::DigitalPinState::Active] = command::raw::Auxiliary2; // NOTE: Shared with arrows
		retVal['F'][input::DigitalPinState::Active] = command::raw::Auxiliary2;
		retVal[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::GameSelect; // NOTE: Shared with arrows
		retVal[shim::VK_OEM_5][input::DigitalPinState::Active] = command::raw::GameStart; // NOTE: Shared with arrows ('\')
	}
	{
		// Arrow keys
		retVal[shim::VK_UP][input::DigitalPinState::Active] = command::raw::Up;
		retVal[shim::VK_UP][input::DigitalPinState::Inactive] = command::raw::UpStop;
		retVal[shim::VK_LEFT][input::DigitalPinState::Active] = command::raw::Left;
		retVal[shim::VK_LEFT][input::DigitalPinState::Inactive] = command::raw::LeftStop;
		retVal[shim::VK_DOWN][input::DigitalPinState::Active] = command::raw::Down;
		retVal[shim::VK_DOWN][input::DigitalPinState::Inactive] = command::raw::DownStop;
		retVal[shim::VK_RIGHT][input::DigitalPinState::Active] = command::raw::Right;
		retVal[shim::VK_RIGHT][input::DigitalPinState::Inactive] = command::raw::RightStop;

		retVal[shim::VK_OEM_4][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with WASD ('[')
		retVal[shim::VK_OEM_6][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with WASD (']')
		retVal[shim::VK_PRIOR][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with WASD
		retVal[shim::VK_NEXT][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with WASD
		retVal[shim::VK_HOME][input::DigitalPinState::Active] = command::raw::Home; // NOTE: Shared with WASD
		retVal[shim::VK_END][input::DigitalPinState::Active] = command::raw::End; // NOTE: Shared with WASD

		retVal[shim::VK_SPACE][input::DigitalPinState::Active] = command::raw::Affirmative;
		retVal[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with WASD
		retVal[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary1; // NOTE: Shared with WASD
		retVal['X'][input::DigitalPinState::Active] = command::raw::Auxiliary2; // NOTE: Shared with WASD
		retVal[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::GameSelect; // NOTE: Shared with WASD
		retVal[shim::VK_OEM_5][input::DigitalPinState::Active] = command::raw::GameStart; // NOTE: Shared with WASD ('\')
	}
	{
		// Hat
		retVal['I'][input::DigitalPinState::Active] = command::raw::HatUp;
		retVal['I'][input::DigitalPinState::Inactive] = command::raw::HatUpStop;
		retVal['J'][input::DigitalPinState::Active] = command::raw::HatLeft;
		retVal['J'][input::DigitalPinState::Inactive] = command::raw::HatLeftStop;
		retVal['K'][input::DigitalPinState::Active] = command::raw::HatDown;
		retVal['K'][input::DigitalPinState::Inactive] = command::raw::HatDownStop;
		retVal['L'][input::DigitalPinState::Active] = command::raw::HatRight;
		retVal['L'][input::DigitalPinState::Inactive] = command::raw::HatRightStop;
	}
	{
		// Developer
		retVal[shim::VK_OEM_3][input::DigitalPinState::Active] = command::raw::DeveloperToggle; // Tilde
		retVal[shim::VK_F1][input::DigitalPinState::Active] = command::raw::DeveloperCycle;
		retVal[shim::VK_F2][input::DigitalPinState::Active] = command::raw::DeveloperAction1;
		retVal[shim::VK_F3][input::DigitalPinState::Active] = command::raw::DeveloperAction2;
		retVal[shim::VK_F4][input::DigitalPinState::Active] = command::raw::DeveloperAction3;
		retVal[shim::VK_F5][input::DigitalPinState::Active] = command::raw::DeveloperAction4;
		retVal[shim::VK_F6][input::DigitalPinState::Active] = command::raw::DeveloperAction5;
	}
	return retVal;
}



input::HotkeyController::CommandMapping HardcodedMenuMapping()
{
	input::HotkeyController::CommandMapping retVal;
	retVal[command::raw::Up] = command::game::UINavigateUp;
	retVal[command::raw::Down] = command::game::UINavigateDown;
	retVal[command::raw::Left] = command::game::UINavigateLeft;
	retVal[command::raw::Right] = command::game::UINavigateRight;
	retVal[command::raw::PgUp] = command::game::UINavigateToPreviousGroup;
	retVal[command::raw::PgDn] = command::game::UINavigateToNextGroup;
	retVal[command::raw::Home] = command::game::UINavigateToFirst;
	retVal[command::raw::End] = command::game::UINavigateToLast;
	retVal[command::raw::Affirmative] = command::game::UIActivateSelection;
	retVal[command::raw::Negative] = command::game::UICancelSelection;
	retVal[command::raw::Auxiliary1] = command::game::UIAuxiliaryAction1;
	retVal[command::raw::Auxiliary2] = command::game::UIAuxiliaryAction2;
	retVal[command::raw::GameSelect] = command::game::UIMenuAction;
	retVal[command::raw::GameStart] = command::game::UIPauseAction;
	return retVal;
}



input::HotkeyController::CommandMapping HardcodedDevMapping()
{
	input::HotkeyController::CommandMapping retVal;
	retVal[command::raw::DeveloperToggle] = command::game::DeveloperToggle;
	retVal[command::raw::DeveloperCycle] = command::game::DeveloperCycle;
	retVal[command::raw::DeveloperAction1] = command::game::DeveloperAction1;
	retVal[command::raw::DeveloperAction2] = command::game::DeveloperAction2;
	retVal[command::raw::DeveloperAction3] = command::game::DeveloperAction3;
	retVal[command::raw::DeveloperAction4] = command::game::DeveloperAction4;
	retVal[command::raw::DeveloperAction5] = command::game::DeveloperAction5;
	return retVal;
}



input::HotkeyController::CommandMapping HardcodedGameMapping()
{
	input::HotkeyController::CommandMapping retVal;
	retVal[command::raw::Up] = command::game::WalkNorth;
	retVal[command::raw::UpStop] = command::game::WalkNorthStop;
	retVal[command::raw::Down] = command::game::WalkSouth;
	retVal[command::raw::DownStop] = command::game::WalkSouthStop;
	retVal[command::raw::Left] = command::game::WalkWest;
	retVal[command::raw::LeftStop] = command::game::WalkWestStop;
	retVal[command::raw::Right] = command::game::WalkEast;
	retVal[command::raw::RightStop] = command::game::WalkEastStop;
	retVal[command::raw::Affirmative] = command::game::Interact;
	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

void HardcodedRawSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	UniquePtr<input::RawInputController> rawController = DefaultCreator<input::RawInputController>::Create();
	rawController->SetLogicalMapping( details::HardcodedRawMapping() );
	details::sRawInputController = rawController;
	manager.StoreRawController( rftl::move( rawController ) );
}



void HardcodedMainSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	// Menus
	UniquePtr<input::HotkeyController> menuHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	menuHotkeyController->SetSource( details::sRawInputController );
	menuHotkeyController->SetCommandMapping( details::HardcodedMenuMapping() );
	UniquePtr<input::RollbackController> menuRollbackController = details::WrapWithRollback( menuHotkeyController, 0 );
	manager.RegisterGameController( menuRollbackController, player::Global, layer::MainMenu );
	manager.StoreGameController( rftl::move( menuHotkeyController ) );
	manager.StoreGameController( rftl::move( menuRollbackController ) );

	// Text
	manager.RegisterTextProvider( details::sRawInputController, player::Global );

	// Developer
	UniquePtr<input::HotkeyController> developerHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	developerHotkeyController->SetSource( details::sRawInputController );
	developerHotkeyController->SetCommandMapping( details::HardcodedDevMapping() );
	manager.RegisterGameController( developerHotkeyController, player::Global, layer::Developer );
	manager.StoreGameController( rftl::move( developerHotkeyController ) );
}



void HardcodedPlayerSetup( PlayerID playerID )
{
	ControllerManager& manager = *app::gInputControllerManager;

	static constexpr uint8_t kStride = 2;
	static_assert( kInvalidPlayerID == 0 );
	RF_ASSERT( playerID != kInvalidPlayerID );
	rollback::InputStreamIdentifier const rollbackStartID = ( ( playerID - 1u ) * kStride ) + 1u;
	rollback::InputStreamIdentifier const rollbackMenusID = rollbackStartID + 0u;
	rollback::InputStreamIdentifier const rollbackGamplayID = rollbackStartID + 1u;

	// Game menus
	UniquePtr<input::HotkeyController> menuHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	menuHotkeyController->SetSource( details::sRawInputController );
	menuHotkeyController->SetCommandMapping( details::HardcodedMenuMapping() );
	UniquePtr<input::RollbackController> menuRollbackController = details::WrapWithRollback( menuHotkeyController, rollbackMenusID );
	manager.RegisterGameController( menuRollbackController, playerID, layer::GameMenu );
	manager.StoreGameController( rftl::move( menuHotkeyController ) );
	manager.StoreGameController( rftl::move( menuRollbackController ) );

	// Gameplay
	UniquePtr<input::HotkeyController> gameplayHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	gameplayHotkeyController->SetSource( details::sRawInputController );
	gameplayHotkeyController->SetCommandMapping( details::HardcodedGameMapping() );
	gameplayHotkeyController->SetSource( details::sRawInputController );
	UniquePtr<input::RollbackController> gameplayRollbackController = details::WrapWithRollback( gameplayHotkeyController, rollbackGamplayID );
	manager.RegisterGameController( gameplayRollbackController, playerID, layer::CharacterControl );
	manager.StoreGameController( rftl::move( gameplayHotkeyController ) );
	manager.StoreGameController( rftl::move( gameplayRollbackController ) );
}



void HardcodedHackSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	// Testing
	UniquePtr<input::HotkeyController> p2HotkeyController = DefaultCreator<input::HotkeyController>::Create();
	p2HotkeyController->SetSource( details::sRawInputController );
	{
		input::HotkeyController::CommandMapping commandMapping;
		commandMapping[command::raw::HatUp] = command::game::WalkNorth;
		commandMapping[command::raw::HatUpStop] = command::game::WalkNorthStop;
		commandMapping[command::raw::HatDown] = command::game::WalkSouth;
		commandMapping[command::raw::HatDownStop] = command::game::WalkSouthStop;
		commandMapping[command::raw::HatLeft] = command::game::WalkWest;
		commandMapping[command::raw::HatLeftStop] = command::game::WalkWestStop;
		commandMapping[command::raw::HatRight] = command::game::WalkEast;
		commandMapping[command::raw::HatRightStop] = command::game::WalkEastStop;
		p2HotkeyController->SetCommandMapping( commandMapping );
	}
	UniquePtr<input::RollbackController> p2RollbackController = details::WrapWithRollback( p2HotkeyController, 3 );
	p2RollbackController->SetArtificialDelay( time::kSimulationFrameDuration * 7 );
	manager.RegisterGameController( p2RollbackController, player::P2, layer::CharacterControl );

	manager.StoreGameController( rftl::move( p2HotkeyController ) );
	manager.StoreGameController( rftl::move( p2RollbackController ) );
}



void HardcodedRawTick()
{
	if( details::sRawInputController != nullptr )
	{
		details::sRawInputController->ConsumeInput( *app::gWndProcInput );
	}
}



PlayerID HardcodedGetLocalPlayer()
{
	return player::P1;
}

///////////////////////////////////////////////////////////////////////////////
}

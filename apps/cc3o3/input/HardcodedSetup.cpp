#include "stdafx.h"
#include "HardcodedSetup.h"

#include "cc3o3/input/InputFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/RawInputController.h"
#include "GameInput/HotkeyController.h"

#include "PlatformInput_win32/WndProcInputDevice.h"

#include "core_platform/winuser_shim.h"
#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace input {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static WeakPtr<input::RawInputController> sRawInputController;

}
///////////////////////////////////////////////////////////////////////////////

void HardcodedSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	UniquePtr<input::RawInputController> rawController = DefaultCreator<input::RawInputController>::Create();
	input::RawInputController::LogicalMapping logicalMapping;
	{
		// WASD
		logicalMapping['W'][input::DigitalPinState::Active] = command::raw::Up;
		logicalMapping['A'][input::DigitalPinState::Active] = command::raw::Left;
		logicalMapping['S'][input::DigitalPinState::Active] = command::raw::Down;
		logicalMapping['D'][input::DigitalPinState::Active] = command::raw::Right;

		logicalMapping[shim::VK_HOME][input::DigitalPinState::Active] = command::raw::Home; // NOTE: Shared with arrows
		logicalMapping[shim::VK_END][input::DigitalPinState::Active] = command::raw::End; // NOTE: Shared with arrows

		logicalMapping[shim::VK_RETURN][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping['E'][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping[shim::VK_BACK][input::DigitalPinState::Active] = command::raw::Negative;
		logicalMapping[shim::VK_OEM_5][input::DigitalPinState::Active] = command::raw::Negative; // '|' or '\'
		logicalMapping[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with arrows
		logicalMapping[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with arrows
		logicalMapping[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary; // NOTE: Shared with arrows
	}
	{
		// Arrow keys
		logicalMapping[shim::VK_UP][input::DigitalPinState::Active] = command::raw::Up;
		logicalMapping[shim::VK_LEFT][input::DigitalPinState::Active] = command::raw::Left;
		logicalMapping[shim::VK_DOWN][input::DigitalPinState::Active] = command::raw::Down;
		logicalMapping[shim::VK_RIGHT][input::DigitalPinState::Active] = command::raw::Right;

		logicalMapping[shim::VK_HOME][input::DigitalPinState::Active] = command::raw::Home; // NOTE: Shared with WASD
		logicalMapping[shim::VK_END][input::DigitalPinState::Active] = command::raw::End; // NOTE: Shared with WASD

		logicalMapping[shim::VK_SPACE][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with WASD
		logicalMapping[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with WASD
		logicalMapping[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary; // NOTE: Shared with WASD
	}
	{
		// Developer
		logicalMapping[shim::VK_OEM_3][input::DigitalPinState::Active] = command::raw::DeveloperToggle; // Tilde
		logicalMapping[shim::VK_F1][input::DigitalPinState::Active] = command::raw::DeveloperCycle;
		logicalMapping[shim::VK_F2][input::DigitalPinState::Active] = command::raw::DeveloperAction1;
		logicalMapping[shim::VK_F3][input::DigitalPinState::Active] = command::raw::DeveloperAction2;
		logicalMapping[shim::VK_F4][input::DigitalPinState::Active] = command::raw::DeveloperAction3;
	}
	rawController->SetLogicalMapping( logicalMapping );

	// Menus
	UniquePtr<input::HotkeyController> menuHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	menuHotkeyController->SetSource( rawController );
	{
		input::HotkeyController::CommandMapping commandMapping;
		commandMapping[command::raw::Up] = command::game::UINavigateUp;
		commandMapping[command::raw::Down] = command::game::UINavigateDown;
		commandMapping[command::raw::Left] = command::game::UINavigateLeft;
		commandMapping[command::raw::Right] = command::game::UINavigateRight;
		commandMapping[command::raw::Home] = command::game::UINavigateToFirst;
		commandMapping[command::raw::End] = command::game::UINavigateToLast;
		commandMapping[command::raw::Affirmative] = command::game::UIActivateSelection;
		commandMapping[command::raw::Negative] = command::game::UICancelSelection;
		menuHotkeyController->SetCommandMapping( commandMapping );
	}
	manager.RegisterGameController( menuHotkeyController, player::P1, layer::MainMenu );
	manager.RegisterGameController( menuHotkeyController, player::P1, layer::GameMenu );

	// Developer
	UniquePtr<input::HotkeyController> developerHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	developerHotkeyController->SetSource( rawController );
	{
		input::HotkeyController::CommandMapping commandMapping;
		commandMapping[command::raw::DeveloperToggle] = command::game::DeveloperToggle;
		commandMapping[command::raw::DeveloperCycle] = command::game::DeveloperCycle;
		commandMapping[command::raw::DeveloperAction1] = command::game::DeveloperAction1;
		commandMapping[command::raw::DeveloperAction2] = command::game::DeveloperAction2;
		commandMapping[command::raw::DeveloperAction3] = command::game::DeveloperAction3;
		developerHotkeyController->SetCommandMapping( commandMapping );
	}
	manager.RegisterGameController( developerHotkeyController, player::P1, layer::Developer );

	details::sRawInputController = rawController;
	manager.StoreRawController( rftl::move( rawController ) );
	manager.StoreGameController( rftl::move( menuHotkeyController ) );
	manager.StoreGameController( rftl::move( developerHotkeyController ) );
}



void HardcodedTick()
{
	if( details::sRawInputController != nullptr )
	{
		details::sRawInputController->ConsumeInput( *app::gWndProcInput );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

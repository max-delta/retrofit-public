#include "stdafx.h"
#include "HardcodedSetup.h"

#include "cc3o3/input/InputFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/RawInputController.h"
#include "GameInput/HotkeyController.h"

#include "core_platform/winuser_shim.h"
#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace input {
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
		logicalMapping[shim::VK_RETURN][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping['E'][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping[shim::VK_BACK][input::DigitalPinState::Active] = command::raw::Negative;
		logicalMapping[shim::VK_OEM_5][input::DigitalPinState::Active] = command::raw::Negative; // '|' or '\'
		logicalMapping[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with WASD
		logicalMapping[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with arrows
		logicalMapping[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary; // NOTE: Shared with arrows
	}
	{
		// Arrow keys
		logicalMapping[shim::VK_UP][input::DigitalPinState::Active] = command::raw::Up;
		logicalMapping[shim::VK_LEFT][input::DigitalPinState::Active] = command::raw::Left;
		logicalMapping[shim::VK_DOWN][input::DigitalPinState::Active] = command::raw::Down;
		logicalMapping[shim::VK_RIGHT][input::DigitalPinState::Active] = command::raw::Right;
		logicalMapping[shim::VK_SPACE][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with WASD
		logicalMapping[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with WASD
		logicalMapping[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary; // NOTE: Shared with WASD
	}
	rawController->SetLogicalMapping( logicalMapping );

	UniquePtr<input::HotkeyController> hotkeyController = DefaultCreator<input::HotkeyController>::Create();
	hotkeyController->SetSource( rawController );

	input::HotkeyController::CommandMapping commandMapping;
	commandMapping[command::raw::Up] = command::game::UINavigateUp;
	commandMapping[command::raw::Down] = command::game::UINavigateDown;
	commandMapping[command::raw::Left] = command::game::UINavigateLeft;
	commandMapping[command::raw::Right] = command::game::UINavigateRight;
	commandMapping[command::raw::Affirmative] = command::game::UIActivateSelection;
	commandMapping[command::raw::Negative] = command::game::UICancelSelection;
	hotkeyController->SetCommandMapping( commandMapping );

	manager.RegisterGameController( hotkeyController, player::P1, layer::MainMenu );
	manager.RegisterGameController( hotkeyController, player::P1, layer::GameMenu );

	manager.StoreRawController( rftl::move(rawController) );
	manager.StoreGameController( rftl::move( hotkeyController ) );
}

///////////////////////////////////////////////////////////////////////////////
}}}

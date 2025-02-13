#include "stdafx.h"
#include "HardcodedSetup.h"

#include "cc3o3/Common.h"
#include "cc3o3/time/TimeFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/RawInputController.h"
#include "GameInput/HotkeyController.h"
#include "GameSync/RollbackController.h"
#include "GameSync/PassthroughController.h"
#include "GameSync/RollbackInputManager.h"

#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformInput_win32/XInputDevice.h"

#include "core_math/math_casts.h"
#include "core_platform/shim/winuser_shim.h"
#include "core/ptr/default_creator.h"

#include "rftl/array"


namespace RF::cc::input {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr size_t kNumXInputDevices = input::XInputDevice::kMaxUserIndex + 1;
static rftl::array<WeakPtr<input::InputDevice>, kNumXInputDevices> sXInputDevices = {};

static WeakPtr<input::RawInputController> sWndProcRawInputController;
static rftl::array<WeakPtr<input::RawInputController>, kNumXInputDevices> sXInputRawInputControllers = {};

static bool sTickSevered = false;



UniquePtr<input::RollbackController> WrapWithRollback(
	WeakPtr<input::GameController> source,
	rollback::InputStreamIdentifier const& identifier )
{
	UniquePtr<input::RollbackController> rollbackController = DefaultCreator<input::RollbackController>::Create();
	rollbackController->SetSource( source );
	gRollbackInputManager->AddController( identifier, rollbackController );
	return rollbackController;
}



UniquePtr<input::PassthroughController> WrapWithPassthrough(
	WeakPtr<input::GameController> source )
{
	UniquePtr<input::PassthroughController> passthroughController = DefaultCreator<input::PassthroughController>::Create();
	passthroughController->SetSource( source );
	gRollbackInputManager->AddPassthrough( passthroughController );
	return passthroughController;
}



input::RawInputController::LogicalMapping HardcodedRawWndProcMapping()
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



input::RawInputController::LogicalMapping HardcodedRawXInputMapping()
{
	using Pin = XInputDigitalInputComponent::Pin;
	static constexpr auto pin = []( XInputDigitalInputComponent::Pin const& in ) -> LogicalCode
	{
		return math::enum_bitcast( in );
	};

	input::RawInputController::LogicalMapping retVal;
	{
		// WASD
		retVal[pin( Pin::DPadUp )][input::DigitalPinState::Active] = command::raw::Up;
		retVal[pin( Pin::DPadUp )][input::DigitalPinState::Inactive] = command::raw::UpStop;
		retVal[pin( Pin::DPadLeft )][input::DigitalPinState::Active] = command::raw::Left;
		retVal[pin( Pin::DPadLeft )][input::DigitalPinState::Inactive] = command::raw::LeftStop;
		retVal[pin( Pin::DPadDown )][input::DigitalPinState::Active] = command::raw::Down;
		retVal[pin( Pin::DPadDown )][input::DigitalPinState::Inactive] = command::raw::DownStop;
		retVal[pin( Pin::DPadRight )][input::DigitalPinState::Active] = command::raw::Right;
		retVal[pin( Pin::DPadRight )][input::DigitalPinState::Inactive] = command::raw::RightStop;

		retVal[pin( Pin::LeftShoulder )][input::DigitalPinState::Active] = command::raw::PgUp;
		retVal[pin( Pin::RightShoulder )][input::DigitalPinState::Active] = command::raw::PgDn;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::Home;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::End;

		retVal[pin( Pin::A )][input::DigitalPinState::Active] = command::raw::Affirmative;
		retVal[pin( Pin::B )][input::DigitalPinState::Active] = command::raw::Negative;
		retVal[pin( Pin::X )][input::DigitalPinState::Active] = command::raw::Auxiliary1;
		retVal[pin( Pin::Y )][input::DigitalPinState::Active] = command::raw::Auxiliary2;
		retVal[pin( Pin::Back )][input::DigitalPinState::Active] = command::raw::GameSelect;
		retVal[pin( Pin::Start )][input::DigitalPinState::Active] = command::raw::GameStart;
	}
	{
		// Hat
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::HatUp;
		//retVal[NONE][input::DigitalPinState::Inactive] = command::raw::HatUpStop;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::HatLeft;
		//retVal[NONE][input::DigitalPinState::Inactive] = command::raw::HatLeftStop;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::HatDown;
		//retVal[NONE][input::DigitalPinState::Inactive] = command::raw::HatDownStop;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::HatRight;
		//retVal[NONE][input::DigitalPinState::Inactive] = command::raw::HatRightStop;
	}
	{
		// Developer
		retVal[pin( Pin::LeftThumb )][input::DigitalPinState::Active] = command::raw::DeveloperToggle;
		retVal[pin( Pin::RightThumb )][input::DigitalPinState::Active] = command::raw::DeveloperCycle;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::DeveloperAction1;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::DeveloperAction2;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::DeveloperAction3;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::DeveloperAction4;
		//retVal[NONE][input::DigitalPinState::Active] = command::raw::DeveloperAction5;
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

void HardcodedDeviceSetup()
{
	// WndProc is so important, that it's always spun up by the common
	//  application framework well before this code should ever run
	RF_ASSERT( app::gWndProcInput );

	// There are a set number of XInput devices, since the API is limited by
	//  console assumptions, so we can just blindly spin them all up
	{
		using input::XInputDevice;
		for( XInputDevice::UserIndex userIndex = 0; userIndex <= XInputDevice::kMaxUserIndex; userIndex++ )
		{
			UniquePtr<input::XInputDevice> device = DefaultCreator<input::XInputDevice>::Create( userIndex );
			WeakPtr<input::InputDevice> ptr = app::gInputControllerManager->StoreInputDevice( rftl::move( device ) );
			RF_ASSERT( ptr != nullptr );
			details::sXInputDevices.at( userIndex ) = rftl::move( ptr );
		}
	}
}



void HardcodedRawSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	// WndProc
	{
		UniquePtr<input::RawInputController> wndProcRawController =
			DefaultCreator<input::RawInputController>::Create(
				app::gWndProcInput->mIdentifier );
		wndProcRawController->SetLogicalMapping( details::HardcodedRawWndProcMapping() );
		details::sWndProcRawInputController = wndProcRawController;
		manager.StoreRawController( rftl::move( wndProcRawController ) );
	}

	// XInput
	static_assert( details::sXInputDevices.size() == details::kNumXInputDevices );
	static_assert( details::sXInputRawInputControllers.size() == details::kNumXInputDevices );
	for( size_t i = 0; i < details::kNumXInputDevices; i++ )
	{
		WeakPtr<input::InputDevice const> const& device = details::sXInputDevices.at( i );
		WeakPtr<input::RawInputController>& controllerSlot = details::sXInputRawInputControllers.at( i );

		UniquePtr<input::RawInputController> xInputRawController =
			DefaultCreator<input::RawInputController>::Create(
				device->mIdentifier );
		xInputRawController->SetLogicalMapping( details::HardcodedRawXInputMapping() );
		controllerSlot = xInputRawController;
		manager.StoreRawController( rftl::move( xInputRawController ) );
	}
}



void HardcodedMainSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	// Menus
	UniquePtr<input::HotkeyController> menuHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	menuHotkeyController->SetSource( details::sWndProcRawInputController );
	menuHotkeyController->SetCommandMapping( details::HardcodedMenuMapping() );
	UniquePtr<input::PassthroughController> menuPassthroughController = details::WrapWithPassthrough( menuHotkeyController );
	manager.RegisterGameController( menuPassthroughController, player::Global, layer::MainMenu );
	manager.StoreGameController( rftl::move( menuHotkeyController ) );
	manager.StoreGameController( rftl::move( menuPassthroughController ) );

	// Text
	manager.RegisterTextProvider( details::sWndProcRawInputController, player::Global );

	// Developer
	UniquePtr<input::HotkeyController> developerHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	developerHotkeyController->SetSource( details::sWndProcRawInputController );
	developerHotkeyController->SetCommandMapping( details::HardcodedDevMapping() );
	manager.RegisterGameController( developerHotkeyController, player::Global, layer::Developer );
	manager.StoreGameController( rftl::move( developerHotkeyController ) );
}



void HardcodedPlayerSetup( PlayerID playerID )
{
	RF_ASSERT( playerID != kInvalidPlayerID );

	ControllerManager& manager = *app::gInputControllerManager;

	HardcodedRollbackIdentifiers const identifiers( playerID );

	// Game menus
	UniquePtr<input::HotkeyController> menuHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	menuHotkeyController->SetSource( details::sWndProcRawInputController );
	menuHotkeyController->SetCommandMapping( details::HardcodedMenuMapping() );
	UniquePtr<input::RollbackController> menuRollbackController = details::WrapWithRollback(
		menuHotkeyController, identifiers.mRollbackGameMenusID );
	manager.RegisterGameController( menuRollbackController, playerID, layer::GameMenu );
	manager.StoreGameController( rftl::move( menuHotkeyController ) );
	manager.StoreGameController( rftl::move( menuRollbackController ) );

	// Gameplay
	UniquePtr<input::HotkeyController> gameplayHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	gameplayHotkeyController->SetSource( details::sWndProcRawInputController );
	gameplayHotkeyController->SetCommandMapping( details::HardcodedGameMapping() );
	gameplayHotkeyController->SetSource( details::sWndProcRawInputController );
	UniquePtr<input::RollbackController> gameplayRollbackController = details::WrapWithRollback(
		gameplayHotkeyController, identifiers.mRollbackGamplayID );
	manager.RegisterGameController( gameplayRollbackController, playerID, layer::CharacterControl );
	manager.StoreGameController( rftl::move( gameplayHotkeyController ) );
	manager.StoreGameController( rftl::move( gameplayRollbackController ) );
}



void HardcodedHackSetup( PlayerID playerID )
{
	ControllerManager& manager = *app::gInputControllerManager;

	HardcodedRollbackIdentifiers const identifiers( playerID );

	// Testing
	UniquePtr<input::HotkeyController> hotkeyController = DefaultCreator<input::HotkeyController>::Create();
	hotkeyController->SetSource( details::sWndProcRawInputController );
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
		hotkeyController->SetCommandMapping( commandMapping );
	}
	UniquePtr<input::RollbackController> rollbackController = details::WrapWithRollback(
		hotkeyController, identifiers.mRollbackGamplayID );
	rollbackController->SetArtificialDelay( time::kSimulationFrameDuration * 7 );
	manager.RegisterGameController( rollbackController, playerID, layer::CharacterControl );

	manager.StoreGameController( rftl::move( hotkeyController ) );
	manager.StoreGameController( rftl::move( rollbackController ) );
}



void HardcodedDeviceTick()
{
	// WndProc
	app::gWndProcInput->OnTick();

	// XInput
	for( WeakPtr<InputDevice> const& device : details::sXInputDevices )
	{
		if( device == nullptr )
		{
			// Might be too early in boot, might not have set up devices yet
			continue;
		}

		device->OnTick();
	}
}



void HardcodedRawTick()
{
	if( details::sTickSevered )
	{
		// This is mostly for debugging, so that the input devices can have
		//  their buffers inspected instead of being consumed
		return;
	}

	// WndProc
	if( details::sWndProcRawInputController != nullptr )
	{
		details::sWndProcRawInputController->ConsumeInput( *app::gWndProcInput );
	}

	// XInput
	static_assert( details::sXInputDevices.size() == details::kNumXInputDevices );
	static_assert( details::sXInputRawInputControllers.size() == details::kNumXInputDevices );
	for( size_t i = 0; i < details::kNumXInputDevices; i++ )
	{
		WeakPtr<input::InputDevice>& device = details::sXInputDevices.at( i );
		if( device == nullptr )
		{
			continue;
		}

		WeakPtr<input::RawInputController>& controllerSlot = details::sXInputRawInputControllers.at( i );
		if( controllerSlot == nullptr )
		{
			continue;
		}

		controllerSlot->ConsumeInput( *device );
	}
}



void HardcodedSeverTick()
{
	details::sTickSevered = true;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "HardcodedSetup.h"

#include "cc3o3/input/InputFwd.h"
#include "cc3o3/time/TimeFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/RawInputController.h"
#include "GameInput/HotkeyController.h"
#include "GameSync/RollbackController.h"
#include "GameSync/RollbackFilters.h"

#include "Rollback/RollbackManager.h"
#include "Rollback/InputStreamRef.h"

#include "PlatformInput_win32/WndProcInputDevice.h"

#include "core_platform/shim/winuser_shim.h"
#include "core/ptr/default_creator.h"


namespace RF::cc::input {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static WeakPtr<input::RawInputController> sRawInputController;
static rftl::vector<WeakPtr<input::RollbackController>> sRollbackControllers;
static rftl::deque<rftl::pair<rollback::InputStreamIdentifier, rollback::InputEvent>> sDebugQueuedTestInput;

}
///////////////////////////////////////////////////////////////////////////////

void HardcodedRawSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	UniquePtr<input::RawInputController> rawController = DefaultCreator<input::RawInputController>::Create();
	input::RawInputController::LogicalMapping logicalMapping;
	{
		// WASD
		logicalMapping['W'][input::DigitalPinState::Active] = command::raw::Up;
		logicalMapping['W'][input::DigitalPinState::Inactive] = command::raw::UpStop;
		logicalMapping['A'][input::DigitalPinState::Active] = command::raw::Left;
		logicalMapping['A'][input::DigitalPinState::Inactive] = command::raw::LeftStop;
		logicalMapping['S'][input::DigitalPinState::Active] = command::raw::Down;
		logicalMapping['S'][input::DigitalPinState::Inactive] = command::raw::DownStop;
		logicalMapping['D'][input::DigitalPinState::Active] = command::raw::Right;
		logicalMapping['D'][input::DigitalPinState::Inactive] = command::raw::RightStop;

		logicalMapping[shim::VK_OEM_4][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with arrows ('[')
		logicalMapping[shim::VK_OEM_6][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with arrows (']')
		logicalMapping[shim::VK_PRIOR][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with arrows
		logicalMapping[shim::VK_NEXT][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with arrows
		logicalMapping[shim::VK_HOME][input::DigitalPinState::Active] = command::raw::Home; // NOTE: Shared with arrows
		logicalMapping[shim::VK_END][input::DigitalPinState::Active] = command::raw::End; // NOTE: Shared with arrows

		logicalMapping[shim::VK_RETURN][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping['E'][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping[shim::VK_BACK][input::DigitalPinState::Active] = command::raw::Negative;
		logicalMapping[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with arrows
		logicalMapping[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary1; // NOTE: Shared with arrows
		logicalMapping['X'][input::DigitalPinState::Active] = command::raw::Auxiliary2; // NOTE: Shared with arrows
		logicalMapping['F'][input::DigitalPinState::Active] = command::raw::Auxiliary2;
		logicalMapping[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::GameSelect; // NOTE: Shared with arrows
		logicalMapping[shim::VK_OEM_5][input::DigitalPinState::Active] = command::raw::GameStart; // NOTE: Shared with arrows ('\')
	}
	{
		// Arrow keys
		logicalMapping[shim::VK_UP][input::DigitalPinState::Active] = command::raw::Up;
		logicalMapping[shim::VK_UP][input::DigitalPinState::Inactive] = command::raw::UpStop;
		logicalMapping[shim::VK_LEFT][input::DigitalPinState::Active] = command::raw::Left;
		logicalMapping[shim::VK_LEFT][input::DigitalPinState::Inactive] = command::raw::LeftStop;
		logicalMapping[shim::VK_DOWN][input::DigitalPinState::Active] = command::raw::Down;
		logicalMapping[shim::VK_DOWN][input::DigitalPinState::Inactive] = command::raw::DownStop;
		logicalMapping[shim::VK_RIGHT][input::DigitalPinState::Active] = command::raw::Right;
		logicalMapping[shim::VK_RIGHT][input::DigitalPinState::Inactive] = command::raw::RightStop;

		logicalMapping[shim::VK_OEM_4][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with WASD ('[')
		logicalMapping[shim::VK_OEM_6][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with WASD (']')
		logicalMapping[shim::VK_PRIOR][input::DigitalPinState::Active] = command::raw::PgUp; // NOTE: Shared with WASD
		logicalMapping[shim::VK_NEXT][input::DigitalPinState::Active] = command::raw::PgDn; // NOTE: Shared with WASD
		logicalMapping[shim::VK_HOME][input::DigitalPinState::Active] = command::raw::Home; // NOTE: Shared with WASD
		logicalMapping[shim::VK_END][input::DigitalPinState::Active] = command::raw::End; // NOTE: Shared with WASD

		logicalMapping[shim::VK_SPACE][input::DigitalPinState::Active] = command::raw::Affirmative;
		logicalMapping[shim::VK_CONTROL][input::DigitalPinState::Active] = command::raw::Negative; // NOTE: Shared with WASD
		logicalMapping[shim::VK_SHIFT][input::DigitalPinState::Active] = command::raw::Auxiliary1; // NOTE: Shared with WASD
		logicalMapping['X'][input::DigitalPinState::Active] = command::raw::Auxiliary2; // NOTE: Shared with WASD
		logicalMapping[shim::VK_TAB][input::DigitalPinState::Active] = command::raw::GameSelect; // NOTE: Shared with WASD
		logicalMapping[shim::VK_OEM_5][input::DigitalPinState::Active] = command::raw::GameStart; // NOTE: Shared with WASD ('\')
	}
	{
		// Hat
		logicalMapping['I'][input::DigitalPinState::Active] = command::raw::HatUp;
		logicalMapping['I'][input::DigitalPinState::Inactive] = command::raw::HatUpStop;
		logicalMapping['J'][input::DigitalPinState::Active] = command::raw::HatLeft;
		logicalMapping['J'][input::DigitalPinState::Inactive] = command::raw::HatLeftStop;
		logicalMapping['K'][input::DigitalPinState::Active] = command::raw::HatDown;
		logicalMapping['K'][input::DigitalPinState::Inactive] = command::raw::HatDownStop;
		logicalMapping['L'][input::DigitalPinState::Active] = command::raw::HatRight;
		logicalMapping['L'][input::DigitalPinState::Inactive] = command::raw::HatRightStop;
	}
	{
		// Developer
		logicalMapping[shim::VK_OEM_3][input::DigitalPinState::Active] = command::raw::DeveloperToggle; // Tilde
		logicalMapping[shim::VK_F1][input::DigitalPinState::Active] = command::raw::DeveloperCycle;
		logicalMapping[shim::VK_F2][input::DigitalPinState::Active] = command::raw::DeveloperAction1;
		logicalMapping[shim::VK_F3][input::DigitalPinState::Active] = command::raw::DeveloperAction2;
		logicalMapping[shim::VK_F4][input::DigitalPinState::Active] = command::raw::DeveloperAction3;
		logicalMapping[shim::VK_F5][input::DigitalPinState::Active] = command::raw::DeveloperAction4;
		logicalMapping[shim::VK_F6][input::DigitalPinState::Active] = command::raw::DeveloperAction5;
	}
	rawController->SetLogicalMapping( logicalMapping );
	details::sRawInputController = rawController;
	manager.StoreRawController( rftl::move( rawController ) );
}



void HardcodedMainSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	// Menus
	UniquePtr<input::HotkeyController> menuHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	menuHotkeyController->SetSource( details::sRawInputController );
	{
		input::HotkeyController::CommandMapping commandMapping;
		commandMapping[command::raw::Up] = command::game::UINavigateUp;
		commandMapping[command::raw::Down] = command::game::UINavigateDown;
		commandMapping[command::raw::Left] = command::game::UINavigateLeft;
		commandMapping[command::raw::Right] = command::game::UINavigateRight;
		commandMapping[command::raw::PgUp] = command::game::UINavigateToPreviousGroup;
		commandMapping[command::raw::PgDn] = command::game::UINavigateToNextGroup;
		commandMapping[command::raw::Home] = command::game::UINavigateToFirst;
		commandMapping[command::raw::End] = command::game::UINavigateToLast;
		commandMapping[command::raw::Affirmative] = command::game::UIActivateSelection;
		commandMapping[command::raw::Negative] = command::game::UICancelSelection;
		commandMapping[command::raw::Auxiliary1] = command::game::UIAuxiliaryAction1;
		commandMapping[command::raw::Auxiliary2] = command::game::UIAuxiliaryAction2;
		commandMapping[command::raw::GameSelect] = command::game::UIMenuAction;
		commandMapping[command::raw::GameStart] = command::game::UIPauseAction;
		menuHotkeyController->SetCommandMapping( commandMapping );
	}
	UniquePtr<input::RollbackController> menuRollbackController = DefaultCreator<input::RollbackController>::Create();
	menuRollbackController->SetSource( menuHotkeyController );
	menuRollbackController->SetRollbackManager( app::gRollbackManager );
	menuRollbackController->SetRollbackIdentifier( 0 );
	app::gRollbackManager->CreateNewStream( 0, time::FrameClock::now() );
	details::sRollbackControllers.emplace_back( menuRollbackController );
	manager.RegisterGameController( menuRollbackController, player::Global, layer::MainMenu );
	manager.StoreGameController( rftl::move( menuHotkeyController ) );
	manager.StoreGameController( rftl::move( menuRollbackController ) );

	// Text
	manager.RegisterTextProvider( details::sRawInputController, player::Global );

	// Developer
	UniquePtr<input::HotkeyController> developerHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	developerHotkeyController->SetSource( details::sRawInputController );
	{
		input::HotkeyController::CommandMapping commandMapping;
		commandMapping[command::raw::DeveloperToggle] = command::game::DeveloperToggle;
		commandMapping[command::raw::DeveloperCycle] = command::game::DeveloperCycle;
		commandMapping[command::raw::DeveloperAction1] = command::game::DeveloperAction1;
		commandMapping[command::raw::DeveloperAction2] = command::game::DeveloperAction2;
		commandMapping[command::raw::DeveloperAction3] = command::game::DeveloperAction3;
		commandMapping[command::raw::DeveloperAction4] = command::game::DeveloperAction4;
		commandMapping[command::raw::DeveloperAction5] = command::game::DeveloperAction5;
		developerHotkeyController->SetCommandMapping( commandMapping );
	}
	manager.RegisterGameController( developerHotkeyController, player::Global, layer::Developer );
	manager.StoreGameController( rftl::move( developerHotkeyController ) );
}



void HardcodedGameSetup()
{
	ControllerManager& manager = *app::gInputControllerManager;

	// Clone main menu controller to P1
	manager.RegisterGameController(
		manager.GetGameController( player::Global, layer::MainMenu ),
		player::P1, layer::GameMenu );

	// Gameplay
	UniquePtr<input::HotkeyController> p1HotkeyController = DefaultCreator<input::HotkeyController>::Create();
	p1HotkeyController->SetSource( details::sRawInputController );
	{
		input::HotkeyController::CommandMapping commandMapping;
		commandMapping[command::raw::Up] = command::game::WalkNorth;
		commandMapping[command::raw::UpStop] = command::game::WalkNorthStop;
		commandMapping[command::raw::Down] = command::game::WalkSouth;
		commandMapping[command::raw::DownStop] = command::game::WalkSouthStop;
		commandMapping[command::raw::Left] = command::game::WalkWest;
		commandMapping[command::raw::LeftStop] = command::game::WalkWestStop;
		commandMapping[command::raw::Right] = command::game::WalkEast;
		commandMapping[command::raw::RightStop] = command::game::WalkEastStop;
		commandMapping[command::raw::Affirmative] = command::game::Interact;
		p1HotkeyController->SetCommandMapping( commandMapping );
	}
	p1HotkeyController->SetSource( details::sRawInputController );
	UniquePtr<input::RollbackController> p1RollbackController = DefaultCreator<input::RollbackController>::Create();
	p1RollbackController->SetSource( p1HotkeyController );
	p1RollbackController->SetRollbackManager( app::gRollbackManager );
	p1RollbackController->SetRollbackIdentifier( 1 );
	app::gRollbackManager->CreateNewStream( 1, time::FrameClock::now() );
	details::sRollbackControllers.emplace_back( p1RollbackController );
	manager.RegisterGameController( p1RollbackController, player::P1, layer::CharacterControl );

	manager.StoreGameController( rftl::move( p1HotkeyController ) );
	manager.StoreGameController( rftl::move( p1RollbackController ) );
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
	UniquePtr<input::RollbackController> p2RollbackController = DefaultCreator<input::RollbackController>::Create();
	p2RollbackController->SetSource( p2HotkeyController );
	p2RollbackController->SetRollbackManager( app::gRollbackManager );
	p2RollbackController->SetRollbackIdentifier( 2 );
	p2RollbackController->SetArtificialDelay( time::kSimulationFrameDuration * 7 );
	app::gRollbackManager->CreateNewStream( 2, time::FrameClock::now() );
	details::sRollbackControllers.emplace_back( p2RollbackController );
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



void HardcodedRollbackTick()
{
	for( WeakPtr<input::RollbackController> const& controller : details::sRollbackControllers )
	{
		controller->ProcessInput( time::FrameClock::now(), time::FrameClock::now() );
	}
}



void HardcodedAdvance( time::CommonClock::time_point lockedFrame, time::CommonClock::time_point newWriteHead )
{
	for( WeakPtr<input::RollbackController> const& controller : details::sRollbackControllers )
	{
		controller->AdvanceInputStream( lockedFrame, newWriteHead );
	}
}



PlayerID HardcodedGetLocalPlayer()
{
	return player::P1;
}



void DebugQueueTestInput( time::CommonClock::time_point frame, rollback::InputStreamIdentifier streamID, rollback::InputValue input )
{
	details::sDebugQueuedTestInput.emplace_back( streamID, rollback::InputEvent( frame, input ) );
}



void DebugSubmitTestInput()
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;
	for( rftl::pair<rollback::InputStreamIdentifier, rollback::InputEvent> const& input : details::sDebugQueuedTestInput )
	{
		rollback::InputStreamRef const stream = sync::RollbackFilters::GetMutableStreamRef( rollMan, input.first );
		bool const valid = sync::RollbackFilters::TryPrepareRemoteFrame( rollMan, stream, input.second.mTime );
		if( valid && input.second.mValue != rollback::kInvalidInputValue )
		{
			stream.mUncommitted.emplace_back( input.second );
		}
	}
	details::sDebugQueuedTestInput.clear();
}



void DebugClearTestInput()
{
	details::sDebugQueuedTestInput.clear();
}

///////////////////////////////////////////////////////////////////////////////
}

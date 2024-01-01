#pragma once
#include "GameInput/InputFwd.h"

#include "core_input/InputFwd.h"
#include "core_time/CommonClock.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/vector"
#include "rftl/unordered_map"
#include "rftl/unordered_set"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

// Keeps track of (and optionally stores) the potentially complex web of
//  controller layers
class GAMEINPUT_API ControllerManager
{
	RF_NO_COPY( ControllerManager );

	//
	// Types and constants
public:
	using PlayerIDs = rftl::unordered_set<PlayerID>;
	using LayerIDs = rftl::unordered_set<LayerID>;

#if RF_IS_ALLOWED( RF_CONFIG_INPUT_DEBUG_ACCESS )
	using DebugPeekRawControllerHandles = rftl::vector<WeakPtr<RawController const>>;
	using DebugPeekGameControllerHandles = rftl::vector<WeakPtr<GameController const>>;
	using DebugPeekInputDeviceHandles = rftl::vector<WeakPtr<InputDevice const>>;
#endif

private:
	using RawControllerStorage = rftl::vector<UniquePtr<RawController>>;
	using GameControllerStorage = rftl::vector<UniquePtr<GameController>>;
	using InputDeviceStorage = rftl::vector<UniquePtr<InputDevice>>;
	using LayerMapping = rftl::unordered_map<LayerID, WeakPtr<GameController>>;
	using PlayerMapping = rftl::unordered_map<PlayerID, LayerMapping>;
	using TextMapping = rftl::unordered_map<PlayerID, WeakPtr<RawController>>;


	//
	// Public methods
public:
	ControllerManager() = default;

	// List of players that can provide input
	PlayerIDs GetRegisteredPlayers() const;

	// List of input layers / channels that input can be sent on
	LayerIDs GetRegisteredLayers() const;

	// At the top of a potentially complex web of controllers, there is a
	//  single game controller interface that is exposed for a given player and
	//  a specified layer
	WeakPtr<GameController const> GetGameController( PlayerID player, LayerID layer ) const;
	void RegisterGameController( WeakPtr<GameController> controller, PlayerID player, LayerID layer );
	WeakPtr<GameController> UnregisterGameController( PlayerID player, LayerID layer );

	// For technical reasons, text is a special class of input that is not
	//  associated with a layer, and is accessed at a much lower level
	WeakPtr<RawController const> GetTextProvider( PlayerID player ) const;
	WeakPtr<RawController> GetMutableTextProvider( PlayerID player ) const;
	void RegisterTextProvider( WeakPtr<RawController> controller, PlayerID player );
	WeakPtr<RawController> UnregisterTextProvider( PlayerID player );

	// It is useful to be able to store underlying input data centrally
	WeakPtr<RawController> StoreRawController( UniquePtr<RawController>&& controller );
	WeakPtr<GameController> StoreGameController( UniquePtr<GameController>&& controller );
	WeakPtr<InputDevice> StoreInputDevice( UniquePtr<InputDevice>&& device );

	// Some complex cases may need to fiddle with time-sensitive buffers
	// NOTE: Networked input rollback and replay is a good example of this
	void TruncateAllRegisteredGameControllers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime );

	// Intended for debug purposes only
#if RF_IS_ALLOWED( RF_CONFIG_INPUT_DEBUG_ACCESS )
	DebugPeekRawControllerHandles DebugPeekRawControllers() const;
	DebugPeekGameControllerHandles DebugPeekGameControllers() const;
	DebugPeekInputDeviceHandles DebugPeekInputDevices() const;
#endif


	//
	// Private data
private:
	RawControllerStorage mRawControllerStorage;
	GameControllerStorage mGameControllerStorage;
	InputDeviceStorage mInputDeviceStorage;
	PlayerMapping mRegisteredGameControllers;
	TextMapping mRegisteredTextProviders;
};

///////////////////////////////////////////////////////////////////////////////
}

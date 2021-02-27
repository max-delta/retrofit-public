#pragma once
#include "GameInput/InputFwd.h"

#include "core_time/CommonClock.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/vector"
#include "rftl/unordered_map"
#include "rftl/unordered_set"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class GAMEINPUT_API ControllerManager
{
	RF_NO_COPY( ControllerManager );

	//
	// Types and constants
public:
	using PlayerIDs = rftl::unordered_set<PlayerID>;
	using LayerIDs = rftl::unordered_set<LayerID>;

private:
	using RawControllerStorage = rftl::vector<UniquePtr<RawController>>;
	using GameControllerStorage = rftl::vector<UniquePtr<GameController>>;
	using LayerMapping = rftl::unordered_map<LayerID, WeakPtr<GameController>>;
	using PlayerMapping = rftl::unordered_map<PlayerID, LayerMapping>;
	using TextMapping = rftl::unordered_map<PlayerID, WeakPtr<RawController>>;


	//
	// Public methods
public:
	ControllerManager() = default;

	PlayerIDs GetRegisteredPlayers() const;
	LayerIDs GetRegisteredLayers() const;
	WeakPtr<GameController> GetGameController( PlayerID player, LayerID layer ) const;
	WeakPtr<RawController> GetTextProvider( PlayerID player ) const;

	void RegisterGameController( WeakPtr<GameController> controller, PlayerID player, LayerID layer );
	WeakPtr<GameController> UnregisterGameController( PlayerID player, LayerID layer );

	void RegisterTextProvider( WeakPtr<RawController> controller, PlayerID player );
	WeakPtr<RawController> UnregisterTextProvider( PlayerID player );

	WeakPtr<RawController> StoreRawController( UniquePtr<RawController>&& controller );
	WeakPtr<GameController> StoreGameController( UniquePtr<GameController>&& controller );

	void TruncateAllRegisteredGameControllers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime );


	//
	// Private data
private:
	RawControllerStorage mRawControllerStorage;
	GameControllerStorage mGameControllerStorage;
	PlayerMapping mRegisteredGameControllers;
	TextMapping mRegisteredTextProviders;
};

///////////////////////////////////////////////////////////////////////////////
}}

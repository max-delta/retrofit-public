#pragma once
#include "GameInput/InputFwd.h"

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


	//
	// Public methods
public:
	ControllerManager() = default;

	PlayerIDs GetRegisteredPlayers() const;
	LayerIDs GetRegisteredLayers() const;
	WeakPtr<GameController> GetGameController( PlayerID player, LayerID layer ) const;

	void RegisterGameController( WeakPtr<GameController> controller, PlayerID player, LayerID layer );
	WeakPtr<GameController> UnregisterGameController( PlayerID player, LayerID layer );

	WeakPtr<RawController> StoreRawController( UniquePtr<RawController>&& controller );
	WeakPtr<GameController> StoreGameController( UniquePtr<GameController>&& controller );


	//
	// Private data
private:
	RawControllerStorage mRawControllerStorage;
	GameControllerStorage mGameControllerStorage;
	PlayerMapping mRegisteredGameControllers;
};

///////////////////////////////////////////////////////////////////////////////
}}

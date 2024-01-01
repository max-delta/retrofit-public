#include "stdafx.h"
#include "ControllerManager.h"

#include "GameInput/GameController.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

ControllerManager::PlayerIDs ControllerManager::GetRegisteredPlayers() const
{
	PlayerIDs retVal;
	for( PlayerMapping::value_type const& player : mRegisteredGameControllers )
	{
		retVal.emplace( player.first );
	}
	return retVal;
}



ControllerManager::LayerIDs ControllerManager::GetRegisteredLayers() const
{
	LayerIDs retVal;
	for( PlayerMapping::value_type const& player : mRegisteredGameControllers )
	{
		for( LayerMapping::value_type const& layer : player.second )
		{
			retVal.emplace( layer.first );
		}
	}
	return retVal;
}



WeakPtr<GameController const> ControllerManager::GetGameController( PlayerID player, LayerID layer ) const
{
	PlayerMapping::const_iterator const playerIter = mRegisteredGameControllers.find( player );
	if( playerIter == mRegisteredGameControllers.end() )
	{
		return nullptr;
	}

	LayerMapping const& layerMapping = playerIter->second;
	LayerMapping::const_iterator const layerIter = layerMapping.find( layer );
	if( layerIter == layerMapping.end() )
	{
		return nullptr;
	}

	return layerIter->second;
}



void ControllerManager::RegisterGameController( WeakPtr<GameController> controller, PlayerID player, LayerID layer )
{
	RF_ASSERT( controller != nullptr );
	LayerMapping& layerMapping = mRegisteredGameControllers[player];
	RF_ASSERT( layerMapping.count( layer ) == 0 );
	layerMapping[layer] = controller;
}



WeakPtr<GameController> ControllerManager::UnregisterGameController( PlayerID player, LayerID layer )
{
	PlayerMapping::iterator const playerIter = mRegisteredGameControllers.find( player );
	if( playerIter == mRegisteredGameControllers.end() )
	{
		return nullptr;
	}

	LayerMapping& layerMapping = playerIter->second;
	LayerMapping::iterator const layerIter = layerMapping.find( layer );
	if( layerIter == layerMapping.end() )
	{
		return nullptr;
	}

	WeakPtr<GameController> const retVal = layerIter->second;
	layerMapping.erase( layerIter );
	if( layerMapping.empty() )
	{
		mRegisteredGameControllers.erase( playerIter );
	}
	return retVal;
}



WeakPtr<RawController const> ControllerManager::GetTextProvider( PlayerID player ) const
{
	return GetMutableTextProvider( player );
}



WeakPtr<RawController> ControllerManager::GetMutableTextProvider( PlayerID player ) const
{
	TextMapping::const_iterator const playerIter = mRegisteredTextProviders.find( player );
	if( playerIter == mRegisteredTextProviders.end() )
	{
		return nullptr;
	}

	return playerIter->second;
}



void ControllerManager::RegisterTextProvider( WeakPtr<RawController> controller, PlayerID player )
{
	RF_ASSERT( controller != nullptr );
	RF_ASSERT( mRegisteredTextProviders.count( player ) == 0 );
	mRegisteredTextProviders[player] = controller;
}



WeakPtr<RawController> ControllerManager::UnregisterTextProvider( PlayerID player )
{
	TextMapping::iterator const playerIter = mRegisteredTextProviders.find( player );
	if( playerIter == mRegisteredTextProviders.end() )
	{
		return nullptr;
	}

	WeakPtr<RawController> const retVal = playerIter->second;
	mRegisteredTextProviders.erase( playerIter );
	return retVal;
}



WeakPtr<RawController> ControllerManager::StoreRawController( UniquePtr<RawController>&& controller )
{
	RF_ASSERT( controller != nullptr );
	WeakPtr<RawController> const retVal = controller;
	mRawControllerStorage.emplace_back( rftl::move( controller ) );
	return retVal;
}



WeakPtr<GameController> ControllerManager::StoreGameController( UniquePtr<GameController>&& controller )
{
	RF_ASSERT( controller != nullptr );
	WeakPtr<GameController> const retVal = controller;
	mGameControllerStorage.emplace_back( rftl::move( controller ) );
	return retVal;
}



WeakPtr<InputDevice> ControllerManager::StoreInputDevice( UniquePtr<InputDevice>&& device )
{
	RF_ASSERT( device != nullptr );
	WeakPtr<InputDevice> const retVal = device;
	mInputDeviceStorage.emplace_back( rftl::move( device ) );
	return retVal;
}



void ControllerManager::TruncateAllRegisteredGameControllers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime )
{
	for( PlayerMapping::value_type& player : mRegisteredGameControllers )
	{
		for( LayerMapping::value_type& layer : player.second )
		{
			RF_ASSERT( layer.second != nullptr );
			layer.second->TruncateBuffers( earliestTime, latestTime );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}

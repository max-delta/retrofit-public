#include "stdafx.h"
#include "ControllerManager.h"


namespace RF { namespace input {
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



WeakPtr<GameController> ControllerManager::GetGameController( PlayerID player, LayerID layer ) const
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

///////////////////////////////////////////////////////////////////////////////
}}

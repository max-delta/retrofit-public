#include "stdafx.h"
#include "SessionMembers.h"

#include "core/macros.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

SessionMembers::PlayerIDs SessionMembers::GetPlayerIDs() const
{
	PlayerIDs retVal;
	retVal.reserve( mPlayerConnections.size() );
	for( PlayerConnections::value_type const& playerConn : mPlayerConnections )
	{
		retVal.emplace( playerConn.first );
	}
	return retVal;
}



SessionMembers::PlayerIDs SessionMembers::GetUnclaimedPlayerIDs() const
{
	PlayerIDs retVal;
	retVal.reserve( mPlayerConnections.size() );
	for( PlayerConnections::value_type const& playerConn : mPlayerConnections )
	{
		if( playerConn.second != kInvalidConnectionIdentifier )
		{
			retVal.emplace( playerConn.first );
		}
	}
	return retVal;
}



input::PlayerID SessionMembers::GetLocalPlayerID() const
{
	if( mLocalConnection == kInvalidConnectionIdentifier )
	{
		return input::kInvalidPlayerID;
	}

	for( PlayerConnections::value_type const& playerConn : mPlayerConnections )
	{
		if( playerConn.second == mLocalConnection )
		{
			return playerConn.first;
		}
	}

	return input::kInvalidPlayerID;
}



SessionMembers::ConnectionPlayerIDs SessionMembers::GetConnectionPlayerIDs() const
{
	ConnectionPlayerIDs retVal;
	retVal.reserve( mAllConnections.size() );
	for( ConnectionIdentifier const& id : mAllConnections )
	{
		retVal[id];
	}
	for( PlayerConnections::value_type const& playerConn : mPlayerConnections )
	{
		ConnectionPlayerIDs::iterator const iter = retVal.find( playerConn.second );
		RF_ASSERT( iter != retVal.end() );
		iter->second.emplace( playerConn.first );
	}
	return retVal;
}



void SessionMembers::ReclaimOrphanedPlayerIDs()
{
	for( PlayerConnections::value_type& playerConn : mPlayerConnections )
	{
		if( mAllConnections.count( playerConn.second ) <= 0 )
		{
			playerConn.second = kInvalidConnectionIdentifier;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}

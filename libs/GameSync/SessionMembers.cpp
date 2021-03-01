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
		if( playerConn.second == kInvalidConnectionIdentifier )
		{
			retVal.emplace( playerConn.first );
		}
	}
	return retVal;
}



SessionMembers::PlayerIDs SessionMembers::GetLocalPlayerIDs() const
{
	PlayerIDs retVal = {};

	if( mLocalConnection == kInvalidConnectionIdentifier )
	{
		return {};
	}

	for( PlayerConnections::value_type const& playerConn : mPlayerConnections )
	{
		if( playerConn.second == mLocalConnection )
		{
			retVal.emplace( playerConn.first );
		}
	}

	return retVal;
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
		if( playerConn.second == kInvalidConnectionIdentifier )
		{
			continue;
		}
		ConnectionPlayerIDs::iterator const iter = retVal.find( playerConn.second );
		RF_ASSERT( iter != retVal.end() );
		iter->second.emplace( playerConn.first );
	}
	return retVal;
}



bool SessionMembers::TryClaimPlayer( input::PlayerID playerID, ConnectionIdentifier newConnID )
{
	RF_ASSERT( playerID != input::kInvalidPlayerID );
	RF_ASSERT( newConnID != kInvalidConnectionIdentifier );
	ConnectionIdentifier& curConnID = mPlayerConnections.at( playerID );

	if( curConnID == newConnID )
	{
		// Already claimed
		return true;
	}

	if( curConnID == kInvalidConnectionIdentifier )
	{
		// Claim
		curConnID = newConnID;
		return true;
	}

	// Claimed by another connection
	return false;
}



bool SessionMembers::TryRelinquishPlayer( input::PlayerID playerID, ConnectionIdentifier oldConnID )
{
	RF_ASSERT( playerID != input::kInvalidPlayerID );
	RF_ASSERT( oldConnID != kInvalidConnectionIdentifier );
	ConnectionIdentifier& curConnID = mPlayerConnections.at( playerID );

	if( curConnID == oldConnID )
	{
		// Relinquish
		curConnID = kInvalidConnectionIdentifier;
		return true;
	}

	// Not claimed by this connection
	// NOTE: May currently be claimed by another connection, the semantics of
	//  this return are that this connection has successfuly ensured that it
	//  holds no claim to the player
	return true;
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

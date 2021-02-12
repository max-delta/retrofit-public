#include "stdafx.h"
#include "SessionMembers.h"


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

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameSync/SyncFwd.h"

#include "GameInput/InputFwd.h"

#include "core_math/Hash.h"

#include "rftl/unordered_set"
#include "rftl/unordered_map"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

class GAMESYNC_API SessionMembers final
{
	//
	// Forwards
private:
	struct Connection;


	//
	// Types and constants
public:
	using Connections = rftl::unordered_set<ConnectionIdentifier, math::DirectHash>;
	using PlayerIDs = rftl::unordered_set<input::PlayerID, math::DirectHash>;
	using PlayerConnections = rftl::unordered_map<input::PlayerID, ConnectionIdentifier, math::DirectHash>;
	using ConnectionPlayerIDs = rftl::unordered_map<ConnectionIdentifier, PlayerIDs, math::DirectHash>;

	//
	// Public methods
public:
	SessionMembers() = default;
	~SessionMembers() = default;

	PlayerIDs GetPlayerIDs() const;
	PlayerIDs GetUnclaimedPlayerIDs() const;
	input::PlayerID GetLocalPlayerID() const;
	ConnectionPlayerIDs GetConnectionPlayerIDs() const;

	void ReclaimOrphanedPlayerIDs();


	//
	// Public data
public:
	Connections mAllConnections;
	PlayerConnections mPlayerConnections;
	ConnectionIdentifier mLocalConnection = kInvalidConnectionIdentifier;
};

///////////////////////////////////////////////////////////////////////////////
}

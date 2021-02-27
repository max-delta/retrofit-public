#pragma once
#include "project.h"

#include "GameSync/SyncFwd.h"
#include "GameSync/protocol/Encryption.h"
#include "GameSync/SessionMembers.h"

#include "core_math/Hash.h"
#include "core_time/SteadyClock.h"

#include "core/ptr/unique_ptr.h"
#include "core/idgen.h"

#include "rftl/functional"
#include "rftl/shared_mutex"
#include "rftl/unordered_map"
#include "rftl/unordered_set"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

// Base class for session managers
class GAMESYNC_API SessionManager
{
	RF_NO_COPY( SessionManager );

	//
	// Forwards
protected:
	struct Connection;
	struct MessageParams;
	struct MessageWorkParams;


	//
	// Types and constants
public:
	// If there are more connections than this, it probably indicates something
	//  has gone wrong, possibly even a malicious client attempting to flood
	//  the connection-handling capabilities, at which point: they've succeeded
	static constexpr size_t kMaxConnectionCount = 32;

protected:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	using Clock = time::SteadyClock;
	using ConnectionIDGen = NonloopingIDGenerator<ConnectionIdentifier>;

	using Connections = rftl::unordered_map<ConnectionIdentifier, Connection, math::DirectHash>;
	using ConnectionIDs = rftl::unordered_set<ConnectionIdentifier, math::DirectHash>;
	using MessagesByRecipient = rftl::unordered_map<ConnectionIdentifier, protocol::Buffer>;

	using OnMessageSig = protocol::ReadResult( MessageParams const& params );
	using OnMessageFunc = rftl::function<OnMessageSig>;
	using DoMessageWorkSig = void( MessageWorkParams const& params );
	using DoMessageWorkFunc = rftl::function<DoMessageWorkSig>;



	//
	// Structs
protected:
	struct Connection
	{
		bool HasPartialHandshake() const;
		bool HasHandshake() const;

		Clock::time_point mInitialConnectionTime = Clock::kLowest;
		Clock::time_point mPartialHandshakeTime = Clock::kLowest;
		Clock::time_point mCompletedHandshakeTime = Clock::kLowest;
		protocol::EncryptionState mEncryption = {};
	};

	struct MessageParams
	{
		RF_NO_COPY( MessageParams );
		ConnectionIdentifier connectionID;
		protocol::MessageID const& messageID;
		rftl::byte_view& bytes;
	};

	struct MessageWorkParams
	{
		RF_NO_COPY( MessageWorkParams );
		ConnectionIDs const& validConnectionIDs;
		MessagesByRecipient& messagesByRecipient;
	};


	//
	// Public methods
public:
	SessionManager();
	~SessionManager() = default;

	// Thread-safe
	SessionMembers GetSessionMembers() const;


	//
	// Protected methods
protected:
	void ProcessPendingConnectionOperations(
		OnMessageFunc const& onMessage,
		DoMessageWorkFunc const& doMessageWork );

	void GetChannels(
		ConnectionIdentifier id,
		SharedPtr<comm::IncomingStream>& incoming,
		SharedPtr<comm::OutgoingStream>& outgoing );


	//
	// Protected data
protected:
	UniquePtr<comm::EndpointManager> const mEndpointManager;

	ConnectionIDGen mConnectionIdentifierGen;

	mutable ReaderWriterMutex mConnectionsMutex;
	Connections mConnections;

	mutable ReaderWriterMutex mSessionMembersMutex;
	SessionMembers mSessionMembers;
};

///////////////////////////////////////////////////////////////////////////////
}

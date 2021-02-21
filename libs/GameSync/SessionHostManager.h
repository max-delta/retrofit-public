#pragma once
#include "project.h"

#include "GameSync/SessionManager.h"
#include "GameSync/SessionMembers.h"
#include "GameSync/protocol/Encryption.h"

#include "core_math/Hash.h"
#include "core_thread/AsyncThread.h"

#include "rftl/unordered_map"
#include "rftl/future"


// Forwards
namespace RF::platform::network {
class TCPSocket;
}

namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

// A host acts as the authoritative owner of the connection list
// NOTE: A host is not required to maintain a session once formed, but
//  new connections will not be able to be accepted
class GAMESYNC_API SessionHostManager final : private SessionManager
{
	RF_NO_COPY( SessionHostManager );

	//
	// Forwards
private:
	struct Connection;


	//
	// Types and constants
public:
	// If there are more connections than this, it probably indicates something
	//  has gone wrong, possibly even a malicious client attempting to flood
	//  the connection-handling capabilities, at which point: they've succeeded
	static constexpr size_t kMaxConnectionCount = 32;

private:
	using Connections = rftl::unordered_map<ConnectionIdentifier, Connection, math::DirectHash>;

	static constexpr rftl::chrono::milliseconds kHandshakeThrottle{ 100 };


	//
	// Structs
public:
	struct HostSpec
	{
		bool mIPv6 = false;
		bool mDevLoopback = false;
		uint16_t mPort = 0;
	};

	struct Diagnostics
	{
		size_t mInvalidConnections = 0;
		size_t mValidConnections = 0;
		SessionMembers mSessionMembers = {};
	};

private:
	struct Connection
	{
		bool HasHandshake() const;

		Clock::time_point mInitialConnectionTime = Clock::kLowest;
		Clock::time_point mIncomingHandshakeTime = Clock::kLowest;
		protocol::EncryptionState mEncryption = {};
	};


	//
	// Public methods
public:
	explicit SessionHostManager( HostSpec spec );
	~SessionHostManager();

	// Thread-safe
	bool IsHostingASession() const;
	void StartHostingASession();
	void StopHostingASession();

	// Thread-safe
	void ProcessPendingOperations();

	// Thread-safe
	Diagnostics ReportDiagnostics() const;


	//
	// Private methods
private:
	void AcceptNewConnection();
	void CreateClientChannels( comm::EndpointIdentifier clientIdentifier, UniquePtr<platform::network::TCPSocket>&& newConnection );
	void GetClientChannels( ConnectionIdentifier id, SharedPtr<comm::IncomingStream>& incoming, SharedPtr<comm::OutgoingStream>& outgoing );
	void ValidateUntrustedConnections();


	//
	// Private data
private:
	mutable ReaderWriterMutex mStartStopMutex;

	HostSpec const mSpec;

	mutable ReaderWriterMutex mListenerSocketMutex;
	UniquePtr<platform::network::TCPSocket> mListenerSocket;

	thread::AsyncThread mListenerThread;
	thread::AsyncThread mValidatorThread;
	rftl::atomic<bool> mLastValidationUneventful = false;

	mutable ReaderWriterMutex mClientConnectionsMutex;
	Connections mClientConnections;

	mutable ReaderWriterMutex mSessionMembersMutex;
	SessionMembers mSessionMembers;
};

///////////////////////////////////////////////////////////////////////////////
}
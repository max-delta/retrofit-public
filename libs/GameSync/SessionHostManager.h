#pragma once
#include "project.h"

#include "GameSync/SessionManager.h"
#include "GameSync/protocol/Encryption.h"

#include "core_thread/AsyncThread.h"

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
class GAMESYNC_API SessionHostManager final : public SessionManager
{
	RF_NO_COPY( SessionHostManager );

	//
	// Types and constants
public:
	// If there are more connections than this, it probably indicates something
	//  has gone wrong, possibly even a malicious client attempting to flood
	//  the connection-handling capabilities, at which point: they've succeeded
	static constexpr size_t kMaxConnectionCount = 32;

private:
	static constexpr rftl::chrono::milliseconds kHandshakeThrottle{ 100 };


	//
	// Structs
public:
	struct HostSpec
	{
		bool mIPv6 = false;
		bool mDevLoopback = false;
		uint16_t mPort = 0;
		SessionMembers::PlayerIDs mPermittedPlayerIDs;
	};

	struct Diagnostics
	{
		size_t mInvalidConnections = 0;
		size_t mValidConnections = 0;
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

	// Thread-safe
	bool AttemptPlayerChange( input::PlayerID id, bool claim );


	//
	// Private methods
private:
	void AcceptNewConnection();
	void CreateClientChannels( comm::EndpointIdentifier clientIdentifier, UniquePtr<platform::network::TCPSocket>&& newConnection );
	void ValidateUntrustedConnections();

	protocol::ReadResult HandleMessage( MessageParams const& params );
	void DoMessageWork( MessageWorkParams const& params );


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

	rftl::atomic<bool> mLocalPlayerChangesMade = false;

	mutable ReaderWriterMutex mProxyChatMessagesMutex;
	ChatMessages mProxyChatMessages;
};

///////////////////////////////////////////////////////////////////////////////
}

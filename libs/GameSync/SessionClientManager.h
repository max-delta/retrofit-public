#pragma once
#include "project.h"

#include "GameSync/SessionManager.h"
#include "GameSync/protocol/Encryption.h"

#include "core_thread/AsyncThread.h"

#include "rftl/future"
#include "rftl/string"


// Forwards
namespace RF::platform::network {
class TCPSocket;
}

namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

// A client treats the connection list of a host as authoritative
// NOTE: A host is not required to maintain a session once formed, but lacking
//  an authority for the connection list prevents it from updating
class GAMESYNC_API SessionClientManager final : private SessionManager
{
	RF_NO_COPY( SessionClientManager );

	//
	// Types and constants
private:
	static constexpr rftl::chrono::milliseconds kHandshakeThrottle{ 100 };

	// NOTE: May add support for multiple hosts in the future, possibly with a
	//  shared ownership model for the total connection list
	static constexpr ConnectionIdentifier kSingleHostIdentifier = 1;
	static constexpr size_t kSingleHostCount = 1;


	//
	// Structs
public:
	struct ClientSpec
	{
		rftl::string mHostname;
		uint16_t mPort = 0;
	};

	struct Diagnostics
	{
		size_t mInvalidConnections = 0;
		size_t mValidConnections = 0;
	};


	//
	// Public methods
public:
	explicit SessionClientManager( ClientSpec spec );
	~SessionClientManager();

	// Thread-safe
	bool IsReceivingASession() const;
	void StartReceivingASession();
	void StopReceivingASession();

	// Thread-safe
	void ProcessPendingOperations();

	// Thread-safe
	Diagnostics ReportDiagnostics() const;


	//
	// Private methods
private:
	void DoHandshakes();

	void GetOrCreateNextHandshakeChannels( SharedPtr<comm::IncomingStream>& incomingStream, SharedPtr<comm::OutgoingStream>& outgoingStream );
	void FormHostConnection( comm::EndpointIdentifier hostIdentifier, ClientSpec spec );
	void CreateHostChannels( comm::EndpointIdentifier hostIdentifier, UniquePtr<platform::network::TCPSocket>&& newConnection );


	//
	// Private data
private:
	mutable ReaderWriterMutex mStartStopMutex;

	ClientSpec const mSpec;

	rftl::atomic<bool> mShouldReceiveASession = false;
	thread::AsyncThread mHandshakeThread;
	rftl::atomic<bool> mLastHandshakeUneventful = false;
};

///////////////////////////////////////////////////////////////////////////////
}

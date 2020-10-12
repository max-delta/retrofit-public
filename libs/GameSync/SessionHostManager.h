#pragma once
#include "project.h"

#include "Communication/CommunicationFwd.h"

#include "core_math/Hash.h"
#include "core_time/SteadyClock.h"
#include "core_thread/AsyncThread.h"

#include "core/ptr/unique_ptr.h"
#include "core/idgen.h"

#include "rftl/unordered_map"
#include "rftl/shared_mutex"
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
class GAMESYNC_API SessionHostManager final
{
	RF_NO_COPY( SessionHostManager );

	//
	// Forwards
private:
	struct Connection;


	//
	// Types and constants
private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	using Clock = time::SteadyClock;
	using ConnectionIdentifier = comm::EndpointIdentifier;
	using ConnectionIDGen = NonloopingIDGenerator<ConnectionIdentifier>;
	using Connections = rftl::unordered_map<ConnectionIdentifier, Connection, math::DirectHash>;


	//
	// Structs
public:
	struct HostSpec
	{
		bool mIPv6 = false;
		bool mDevLoopback = false;
		uint16_t mPort = 0;
	};

private:
	struct Connection
	{
		Clock::time_point mLatestValidInboundData = Clock::kLowest;
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


	//
	// Private methods
private:
	void AcceptNewConnection();


	//
	// Private data
private:
	HostSpec const mSpec;

	mutable ReaderWriterMutex mListenerSocketMutex;
	UniquePtr<platform::network::TCPSocket> mListenerSocket;

	mutable ReaderWriterMutex mListenerThreadMutex;
	thread::AsyncThread mListenerThread;

	mutable ReaderWriterMutex mEndpointManangerMutex;
	UniquePtr<comm::EndpointManager> mEndpointMananger;

	ConnectionIDGen mConnectionIdentifierGen;

	mutable ReaderWriterMutex mClientConnectionsMutex;
	Connections mClientConnections;
};

///////////////////////////////////////////////////////////////////////////////
}

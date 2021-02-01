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
class GAMESYNC_API SessionClientManager final
{
	RF_NO_COPY( SessionClientManager );

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

	// NOTE: May add support for multiple hosts in the future, possibly with a
	//  shared ownership model for the total connection list
	static constexpr ConnectionIdentifier kSingleHostIdentifier = 1;


	//
	// Structs
public:
	struct ClientSpec
	{
		rftl::string mHostname;
		uint16_t mPort = 0;
	};

private:
	struct Connection
	{
		Clock::time_point mInitialConnectionTime = Clock::kLowest;
		Clock::time_point mLatestValidOutboundData = Clock::kLowest;
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


	//
	// Private methods
private:
	void ReceiveUpdate();

	void GetOrCreateNextUpdateChannels( SharedPtr<comm::IncomingStream>& incomingStream, SharedPtr<comm::OutgoingStream>& outgoingStream );
	void FormHostConnection( comm::EndpointIdentifier hostIdentifier, ClientSpec spec );
	void CreateHostChannels( comm::EndpointIdentifier hostIdentifier, UniquePtr<platform::network::TCPSocket>&& newConnection );


	//
	// Private data
private:
	mutable ReaderWriterMutex mStartStopMutex;

	ClientSpec const mSpec;

	rftl::atomic<bool> mShouldReceiveASession = false;

	thread::AsyncThread mUpdateThread;

	UniquePtr<comm::EndpointManager> const mEndpointManager;

	ConnectionIDGen mConnectionIdentifierGen;

	mutable ReaderWriterMutex mHostConnectionsMutex;
	Connections mHostConnections;
};

///////////////////////////////////////////////////////////////////////////////
}

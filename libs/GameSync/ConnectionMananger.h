#pragma once
#include "project.h"

#include "Communication/CommunicationFwd.h"

#include "core_math/Hash.h"
#include "core_time/SteadyClock.h"

#include "core/ptr/unique_ptr.h"
#include "core/idgen.h"

#include "rftl/optional"
#include "rftl/string"
#include "rftl/unordered_map"
#include "rftl/shared_mutex"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

class GAMESYNC_API ConnectionManager final
{
	RF_NO_COPY( ConnectionManager );

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
	struct JoinSpec
	{
		rftl::string mHostname;
	};

private:
	struct Connection
	{
		Clock::time_point mFirstOutboundData = Clock::kLowest;
		Clock::time_point mFirstValidInboundData = Clock::kLowest;
	};


	//
	// Public methods
public:
	ConnectionManager() = default;

	// A host acts as the authoritative owner of the connection list
	// NOTE: A host is not required to maintain a session once formed, but
	//  new connections will not be able to be accepted
	bool IsHostingASession() const;
	void StartHostingASession( HostSpec spec );
	void StopHostingASession();

	// To enter into a session, a participant must submit to the authority of
	//  a host w.r.t. the connection list
	// NOTE: Submitting to a host is not required to maintain a session once
	//  formed, but new connections will not be able to be accepted
	bool IsAcceptingASession() const;
	void StartAcceptingASession( JoinSpec spec );
	void StopAcceptingASession();


	//
	// Private data
private:
	mutable ReaderWriterMutex mCommonMutex;

	UniquePtr<comm::EndpointManager> mEndpointMananger;

	ConnectionIDGen mConnectionIdentifierGen;
	Connections mPeerConnections;

	struct AsHost
	{
		RF_NO_COPY( AsHost );
		AsHost() = default;

		HostSpec mSpec = {};
	};
	rftl::optional<AsHost> mAsHost;

	struct AsJoin
	{
		RF_NO_COPY( AsJoin );
		AsJoin() = default;

		JoinSpec mSpec = {};
		ConnectionIdentifier mHostConnectionIdentifier = ConnectionIDGen::kInvalid;
		Connection mHostConnection = {};
	};
	rftl::optional<AsJoin> mAsJoin;
};

///////////////////////////////////////////////////////////////////////////////
}

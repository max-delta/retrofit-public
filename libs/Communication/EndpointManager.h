#pragma once
#include "project.h"

#include "Communication/LogicalEndpoint.h"

#include "core_math/Hash.h"

#include "rftl/unordered_map"


// Forwards
namespace RF::platform {
class IncomingBufferStitcher;
class OutgoingBufferStream;
}

namespace RF::comm {
///////////////////////////////////////////////////////////////////////////////

class COMMUNICATION_API EndpointManager
{
	RF_NO_COPY( EndpointManager );

	//
	// Types and constants
public:
	using InStreams = rftl::vector<SharedPtr<IncomingStream>>;
	using OutStreams = rftl::vector<SharedPtr<OutgoingStream>>;

private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	using LogicalEndpoints = rftl::unordered_map<EndpointIdentifier, SharedPtr<LogicalEndpoint>, math::DirectHash>;


	//
	// Public methods
public:
	// NOTE: Endpoints are allowed to share streams if desired
	WeakSharedPtr<IncomingStream> AddIncomingStream( SharedPtr<IncomingStream>&& stream );
	WeakSharedPtr<OutgoingStream> AddOutgoingStream( SharedPtr<OutgoingStream>&& stream );
	SharedPtr<IncomingStream> RemoveIncomingStream( WeakPtr<IncomingStream> const& stream );
	SharedPtr<OutgoingStream> RemoveOutgoingStream( WeakPtr<OutgoingStream> const& stream );

	WeakSharedPtr<LogicalEndpoint> AddEndpoint( EndpointIdentifier identifier );
	WeakSharedPtr<LogicalEndpoint> GetEndpoint( EndpointIdentifier identifier );
	void RemoveEndpoint( EndpointIdentifier identifier );

	// Removes all streams not associated with any endpoints
	void RemoveOrphanedStreams( InStreams& incoming, OutStreams& outgoing );


	//
	// Private data
private:
	mutable ReaderWriterMutex mCommonMutex;
	InStreams mInStreams;
	OutStreams mOutStreams;
	LogicalEndpoints mLogicalEndpoints;
};

///////////////////////////////////////////////////////////////////////////////
}

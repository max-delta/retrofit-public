#pragma once
#include "project.h"

#include "GameSync/SyncFwd.h"

#include "core_time/SteadyClock.h"

#include "core/ptr/unique_ptr.h"
#include "core/idgen.h"

#include "rftl/shared_mutex"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

// Base class for session managers
class GAMESYNC_API SessionManager
{
	RF_NO_COPY( SessionManager );

	//
	// Types and constants
protected:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	using Clock = time::SteadyClock;
	using ConnectionIDGen = NonloopingIDGenerator<ConnectionIdentifier>;


	//
	// Public methods
public:
	SessionManager();
	~SessionManager() = default;


	//
	// Protected methods
protected:
	void GetChannels(
		ConnectionIdentifier id,
		SharedPtr<comm::IncomingStream>& incoming,
		SharedPtr<comm::OutgoingStream>& outgoing );


	//
	// Protected data
protected:
	UniquePtr<comm::EndpointManager> const mEndpointManager;

	ConnectionIDGen mConnectionIdentifierGen;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "Rollback/Domain.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

class ROLLBACK_API RollbackManager
{
	RF_NO_COPY( RollbackManager );
	RF_NO_MOVE( RollbackManager );

	//
	// Public methods
public:
	RollbackManager();
	~RollbackManager() = default;

	// The shared domain contains state that is shared with rollback peers, and
	//  can thus suffer from corrections directly induced by peers
	Domain const& GetSharedDomain() const;
	Domain& GetMutableSharedDomain();

	// The private domain contains state that is unknown to rollback peers, and
	//  so will never push corrections to them
	// NOTE: The private domain can still be affected by corrections caused by
	//  peer changes to the shared domain, if some private state is dependent
	//  on shared state
	Domain const& GetPrivateDomain() const;
	Domain& GetMutablePrivateDomain();

	// Taking/loading snapshots affects all domains, and requires all domains
	//  to share the same snapshot points
	// NOTE: Taking snapshots on individual domains will likely cause these
	//  calls to fail
	void TakeAutoSnapshot( time::CommonClock::time_point time );
	time::CommonClock::time_point LoadLatestAutoSnapshot();
	void TakeManualSnapshot( ManualSnapshotIdentifier const& identifier, time::CommonClock::time_point time );
	time::CommonClock::time_point LoadManualSnapshot( ManualSnapshotIdentifier const& identifier );

	// Various clocks for coordinating rollback logic
	//  * The head clock is the leading-edge of the window
	//  * The window clock is the trailing-edge of the window
	//  * The tail clock is the oldest valit time of input
	// NOTE: This has no direct effect on any underlying machinery, and is
	//  primarily intended for external users
	// NOTE: Times outside valid ranges may exist within the underlying
	//  machinery, and are expected if the windows have moved
	time::CommonClock::time_point GetHeadClock() const;
	void SetHeadClock( time::CommonClock::time_point time );
	time::CommonClock::time_point GetWindowClock() const;
	void SetWindowClock( time::CommonClock::time_point time );
	time::CommonClock::time_point GetTailClock() const;
	void SetTailClock( time::CommonClock::time_point time );


	//
	// Private data
private:
	Domain mSharedDomain;
	Domain mPrivateDomain;

	time::CommonClock::time_point mHeadClock;
	time::CommonClock::time_point mWindowClock;
	time::CommonClock::time_point mTailClock;
};

///////////////////////////////////////////////////////////////////////////////
}}

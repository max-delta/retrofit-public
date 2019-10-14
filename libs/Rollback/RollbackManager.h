#pragma once
#include "project.h"

#include "Rollback/Domain.h"
#include "Rollback/InputStream.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

class ROLLBACK_API RollbackManager
{
	RF_NO_COPY( RollbackManager );
	RF_NO_MOVE( RollbackManager );

	//
	// Types and constants
public:
	using InputStreams = rftl::unordered_map<InputStreamIdentifier, InputStream>;


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
	// NOTE: This has no direct effect on domains, and is primarily intended
	//  for external users
	// NOTE: Times outside valid ranges may exist within the underlying
	//  machinery, and are expected if the windows have moved
	time::CommonClock::time_point GetHeadClock() const;
	void SetHeadClock( time::CommonClock::time_point time );
	time::CommonClock::time_point GetWindowClock() const;
	void SetWindowClock( time::CommonClock::time_point time );
	time::CommonClock::time_point GetTailClock() const;
	void SetTailClock( time::CommonClock::time_point time );

	// Committed streams are intended to store input for frames that have
	//  representation from all interested parties. Specifically: the frames
	//  present in the committed streams should never receive corrections or
	//  otherwise be ammended.
	InputStreams const& GetCommittedStreams() const;

	// Uncommitted streams are intended so store input for frames that do not
	//  yet have representation from all interested parties. Specifically: the
	//  frames present in the uncommitted streams can be corrected or otherwise
	//  be ammended.
	// NOTE: It is possible to put the uncommitted streams into an invalid
	//  state such as overlapping with committed frames, which will be detected
	//  on commit and likely result in a fatal error
	InputStreams const& GetUncommittedStreams() const;
	InputStreams& GetMutableUncommittedStreams();

	// Frames that are present in all uncommitted streams are 'ready to commit'
	InclusiveTimeRange GetFramesReadyToCommit() const;

	// Committing frames destructively transfers input from the uncommitted
	//  streams, and blocks further attempts to write new inputs to those
	//  committed frames
	void CommitFrames( InclusiveTimeRange const& range );

	// Destructively rewind all streams to an older time point, effectively
	//  invalidating committed frames and creating a de-sync against all other
	//  peers that were involved in those frames
	// NOTE: Intended to be used for save/load scenarios, since a total state
	//  transfer is needed to overcome the de-syncs caused by this call
	// NOTE: Example use-cases -
	//  * Single-player save-states
	//  * Multi-player save-states with re-join machinery
	//  * TAS single-step frame rewind with state fix-up
	void RewindAllStreams( time::CommonClock::time_point time );


	//
	// Private data
private:
	Domain mSharedDomain;
	Domain mPrivateDomain;

	InputStreams mCommittedStreams;
	InputStreams mUncommittedStreams;

	time::CommonClock::time_point mHeadClock;
	time::CommonClock::time_point mWindowClock;
	time::CommonClock::time_point mTailClock;
};

///////////////////////////////////////////////////////////////////////////////
}}

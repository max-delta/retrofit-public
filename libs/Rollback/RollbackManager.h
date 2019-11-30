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

	// Destructively rewind all domains to an older time point, effectively
	//  undoing all state modifications 
	// NOTE: Intended to be used for rollback scenarios, where a re-simulation
	//  needs to be run, and may not touch all values that a previous
	//  simulation may have written to
	void RewindAllDomains( time::CommonClock::time_point time );

	// The head clock is the leading-edge of the window
	// NOTE: This has no direct effect on domains, and is primarily intended
	//  for external users
	// NOTE: Times outside valid ranges may exist within the underlying
	//  machinery, and are expected if the window has been moved
	time::CommonClock::time_point GetHeadClock() const;
	void SetHeadClock( time::CommonClock::time_point time );

	// Committed streams are intended to store input for frames that have
	//  representation from all interested parties. Specifically: the frames
	//  present in the committed streams should never receive corrections or
	//  otherwise be ammended.
	InputStreams const& GetCommittedStreams() const;
	time::CommonClock::time_point GetMaxCommitHead() const;

	// Uncommitted streams are intended to store input for frames that do not
	//  yet have representation from all interested parties. Specifically: the
	//  frames present in the uncommitted streams can be corrected or otherwise
	//  be ammended.
	// NOTE: It is possible to put the uncommitted streams into an invalid
	//  state such as overlapping with committed frames, which will be detected
	//  on commit and likely result in a fatal error
	InputStreams const& GetUncommittedStreams() const;
	InputStreams& GetMutableUncommittedStreams();

	// Streams that are created late will have their commit time set to match
	//  the streams that existed before
	void CreateNewStream( InputStreamIdentifier const& identifier, time::CommonClock::time_point const& headTime );

	// Frames that are present in all uncommitted streams are 'ready to commit'
	InclusiveTimeRange GetFramesReadyToCommit() const;

	// Committing frames destructively transfers input from the uncommitted
	//  streams, and blocks further attempts to write new inputs to those
	//  committed frames
	void CommitFrames( InclusiveTimeRange const& range, time::CommonClock::time_point const& nextFrame );

	// Destructively rewind all streams to an older time point, effectively
	//  invalidating committed frames and creating a de-sync against all other
	//  peers that were involved in those frames
	// NOTE: Intended to be used for save/load scenarios, since a total state
	//  transfer is needed to overcome the de-syncs caused by this call
	// NOTE: Example use-cases
	//  * Single-player save-states
	//  * Multi-player save-states with re-join machinery
	//  * TAS single-step frame rewind with state fix-up
	// NOTE: Remove is a more aggressive form of rewind, with similar concerns
	void RewindAllStreams( time::CommonClock::time_point time );
	void RemoveAllStreams();

	// Left unbounded, committed inputs can grow to significant sizes that may
	//  begin to impact performance, either from memory pressure or increased
	//  binary search costs. These inputs should be trimmed once they are no
	//  longer necessary for critical-path functionality.
	// NOTE: Expectation is that old inputs are only useful for replay logic,
	//  in which case they can be flushed to disk before trimming and later
	//  re-inserted in batches as uncommitted inputs to go through the normal
	//  logic again
	void TrimOldCommittedInputs( time::CommonClock::time_point time );

	// Snapshots may no longer be interesting or may be invalidated based on
	//  the time they were taken
	size_t ClearAllSnapshotsBefore( time::CommonClock::time_point time );
	size_t ClearAllSnapshotsAfter( time::CommonClock::time_point time );


	//
	// Private data
private:
	Domain mSharedDomain;
	Domain mPrivateDomain;

	InputStreams mCommittedStreams;
	InputStreams mUncommittedStreams;

	time::CommonClock::time_point mHeadClock;
};

///////////////////////////////////////////////////////////////////////////////
}}

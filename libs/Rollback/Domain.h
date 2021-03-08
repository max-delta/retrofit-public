#pragma once
#include "project.h"

#include "Rollback/RollbackFwd.h"

#include "core_state/StateBag.h"
#include "core_allocate/Allocator.h"
#include "core_time/CommonClock.h"

#include "rftl/array"
#include "rftl/unordered_map"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

class ROLLBACK_API Domain
{
	RF_NO_COPY( Domain );
	RF_NO_MOVE( Domain );

	//
	// Types and constants
public:
	using AutoSnapshots = rftl::array<Snapshot, 2>;
	using ManualSnapshots = rftl::unordered_map<ManualSnapshotIdentifier, UniquePtr<Snapshot>>;


	//
	// Public methods
public:
	Domain( alloc::Allocator& windowAllocator, alloc::Allocator& snapshotAllocator );
	~Domain() = default;

	// The high-performance rollback window that has unreliable state, but is
	//  quick to read or mutate, in contrast to state before the window, which
	//  is reliable, but expensive to reconstruct
	Window const& GetWindow() const;
	Window& GetMutableWindow();

	// Automatic snapshots are intermittently written past the window, so that
	//  they can be hashed and compared against peers, or used to bootstrap a
	//  new peer for join-in-progress
	// NOTE: There are two snapshots, so that if the latest one is found to be
	//  invalid, the previous one can be used to help debug it
	void TakeAutoSnapshot( time::CommonClock::time_point time );
	Snapshot const& GetOldestAutoSnapshot() const;
	Snapshot const& GetLatestAutoSnapshot() const;

	// Manual snapshots are for classical savestates or extended rollbacks
	WeakPtr<Snapshot const> TakeManualSnapshot( ManualSnapshotIdentifier const& identifier, time::CommonClock::time_point time );
	WeakPtr<Snapshot const> GetManualSnapshot( ManualSnapshotIdentifier const& identifier ) const;
	UniquePtr<Snapshot> RemoveManualSnapshot( ManualSnapshotIdentifier const& identifier );

	// Loading a snapshot writes to the window, which is usually a destructive
	//  action that causes state to be lost and require re-simulation in order
	//  to restore the lost state
	// NOTE: The re-simulation is expected to normally be paired with a change
	//  in inputs, so that the resulting state ends up being different
	time::CommonClock::time_point LoadSnapshot( Snapshot const& snapshot );

	// Snapshots may no longer be interesting or may be invalidated based on
	//  the time they were taken
	size_t ClearAllSnapshotsBefore( time::CommonClock::time_point time );
	size_t ClearAllSnapshotsAfter( time::CommonClock::time_point time );


	//
	// Private data
private:
	alloc::Allocator& mWindowAllocator;
	Window mWindow;

	alloc::Allocator& mSnapshotAllocator;
	AutoSnapshots mAutoSnapshots;
	ManualSnapshots mManualSnapshots;
};

///////////////////////////////////////////////////////////////////////////////
}

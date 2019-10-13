#include "stdafx.h"
#include "Domain.h"

#include "Rollback/TransferHelpers.h"

#include "core_allocate/DefaultAllocCreator.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

Domain::Domain( alloc::Allocator& windowAllocator, alloc::Allocator& snapshotAllocator )
	: mWindowAllocator( windowAllocator )
	, mSnapshotAllocator( snapshotAllocator )
{
	//
}



Window const& Domain::GetWindow() const
{
	return mWindow;
}



Window& Domain::GetMutableWindow()
{
	return mWindow;
}



void Domain::TakeAutoSnapshot( time::CommonClock::time_point time )
{
	Snapshot const& constSnapshot = GetOldestAutoSnapshot();
	Snapshot& mutableSnapshot = const_cast<Snapshot&>( constSnapshot );
	mutableSnapshot.first = time;
	WriteToSnapshot( mutableSnapshot, mWindow, time, mSnapshotAllocator );
}



Snapshot const& Domain::GetOldestAutoSnapshot() const
{
	if( mAutoSnapshots.front().first < mAutoSnapshots.back().first )
	{
		return mAutoSnapshots.front();
	}
	else
	{
		return mAutoSnapshots.back();
	}
}



Snapshot const& Domain::GetLatestAutoSnapshot() const
{
	if( mAutoSnapshots.front().first < mAutoSnapshots.back().first )
	{
		return mAutoSnapshots.back();
	}
	else
	{
		return mAutoSnapshots.front();
	}
}



WeakPtr<Snapshot const> Domain::TakeManualSnapshot( ManualSnapshotIdentifier const& identifier, time::CommonClock::time_point time )
{
	// Re-use existing snapshot if already present, otherwise create
	UniquePtr<Snapshot>& snapshot = mManualSnapshots[identifier];
	if( snapshot == nullptr )
	{
		snapshot = alloc::DefaultAllocCreator<Snapshot>::Create( mSnapshotAllocator );
	}
	snapshot->first = time;
	WriteToSnapshot( *snapshot, mWindow, time, mSnapshotAllocator );
	return snapshot;
}



WeakPtr<Snapshot const> Domain::GetManualSnapshot( ManualSnapshotIdentifier const& identifier ) const
{
	ManualSnapshots::const_iterator const iter = mManualSnapshots.find( identifier );
	if( iter == mManualSnapshots.end() )
	{
		return nullptr;
	}
	return iter->second;
}



UniquePtr<Snapshot> Domain::RemoveManualSnapshot( ManualSnapshotIdentifier const& identifier )
{
	ManualSnapshots::iterator const iter = mManualSnapshots.find( identifier );
	if( iter == mManualSnapshots.end() )
	{
		return nullptr;
	}
	UniquePtr<Snapshot> retVal = rftl::move( iter->second );
	mManualSnapshots.erase( iter );
	return retVal;
}



void Domain::LoadSnapshot( Snapshot const& snapshot )
{
	ReadFromSnapshot( mWindow, snapshot, mWindowAllocator );
}

///////////////////////////////////////////////////////////////////////////////
}}

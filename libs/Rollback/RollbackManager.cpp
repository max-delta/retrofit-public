#include "stdafx.h"
#include "RollbackManager.h"

#include "Allocation/AccessorDeclaration.h"
#include "Logging/Logging.h"

#include "rftl/limits"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

RollbackManager::RollbackManager()
	: mSharedDomain( *alloc::GetAllocator<RFTAG_ROLLBACK_SHARED_STATE>(), *alloc::GetAllocator<RFTAG_ROLLBACK_SNAPSHOTS>() )
	, mPrivateDomain( *alloc::GetAllocator<RFTAG_ROLLBACK_PRIVATE_STATE>(), *alloc::GetAllocator<RFTAG_ROLLBACK_SNAPSHOTS>() )
{
	//
}



Domain const& RollbackManager::GetSharedDomain() const
{
	return mSharedDomain;
}



Domain& RollbackManager::GetMutableSharedDomain()
{
	return mSharedDomain;
}



Domain const& RollbackManager::GetPrivateDomain() const
{
	return mPrivateDomain;
}



Domain& RollbackManager::GetMutablePrivateDomain()
{
	return mPrivateDomain;
}



void RollbackManager::TakeAutoSnapshot( time::CommonClock::time_point time )
{
	GetMutableSharedDomain().TakeAutoSnapshot( time );
	GetMutablePrivateDomain().TakeAutoSnapshot( time );
	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Saved auto snapshot" );
}



time::CommonClock::time_point RollbackManager::LoadLatestAutoSnapshot()
{
	Domain& share = GetMutableSharedDomain();
	Domain& priv = GetMutablePrivateDomain();

	time::CommonClock::time_point const sharedTime = share.LoadSnapshot( share.GetLatestAutoSnapshot() );
	time::CommonClock::time_point const privateTime = priv.LoadSnapshot( priv.GetLatestAutoSnapshot() );
	RFLOG_TEST_AND_FATAL( sharedTime == privateTime, nullptr, RFCAT_ROLLBACK, "Failed to load consistent auto snapshot" );

	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Loaded auto snapshot" );
	return sharedTime;
}



void RollbackManager::TakeManualSnapshot( ManualSnapshotIdentifier const& identifier, time::CommonClock::time_point time )
{
	GetMutableSharedDomain().TakeManualSnapshot( identifier, time );
	GetMutablePrivateDomain().TakeManualSnapshot( identifier, time );
	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Saved manual snapshot \"%s\"", identifier.c_str() );
}



time::CommonClock::time_point RollbackManager::LoadManualSnapshot( ManualSnapshotIdentifier const& identifier )
{
	Domain& share = GetMutableSharedDomain();
	Domain& priv = GetMutablePrivateDomain();

	WeakPtr<Snapshot const> sharedSnap = share.GetManualSnapshot( identifier );
	RFLOG_TEST_AND_FATAL( sharedSnap != nullptr, nullptr, RFCAT_ROLLBACK, "Failed to shared snapshot" );
	WeakPtr<Snapshot const> privateSnap = priv.GetManualSnapshot( identifier );
	RFLOG_TEST_AND_FATAL( privateSnap != nullptr, nullptr, RFCAT_ROLLBACK, "Failed to private snapshot" );

	time::CommonClock::time_point const sharedTime = share.LoadSnapshot( *sharedSnap );
	time::CommonClock::time_point const privateTime = priv.LoadSnapshot( *privateSnap );
	RFLOG_TEST_AND_FATAL( sharedTime == privateTime, nullptr, RFCAT_ROLLBACK, "Failed to load consistent manual snapshot" );

	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Loaded manual snapshot \"%s\"", identifier.c_str() );
	return sharedTime;
}



time::CommonClock::time_point RollbackManager::GetHeadClock() const
{
	return mHeadClock;
}



void RollbackManager::SetHeadClock( time::CommonClock::time_point time )
{
	mHeadClock = time;
}



RollbackManager::InputStreams const& RollbackManager::GetCommittedStreams() const
{
	return mCommittedStreams;
}



RollbackManager::InputStreams const& RollbackManager::GetUncommittedStreams() const
{
	return mUncommittedStreams;
}



RollbackManager::InputStreams& RollbackManager::GetMutableUncommittedStreams()
{
	return mUncommittedStreams;
}



void RollbackManager::CreateNewStream( InputStreamIdentifier const& identifier, time::CommonClock::time_point const& headTime )
{
	RFLOG_TEST_AND_FATAL( mCommittedStreams.count( identifier ) == 0, nullptr, RFCAT_ROLLBACK, "Trying to create a stream that already exists" );
	RFLOG_TEST_AND_FATAL( mUncommittedStreams.count( identifier ) == 0, nullptr, RFCAT_ROLLBACK, "Trying to create a stream that already exists" );

	// The current commit time is shared by all committed streams, so this new
	//  stream will need to respect that as well
	// NOTE: Getting the first stream, since any are valid, but making sure to
	//  do this before adding the new stream
	time::CommonClock::time_point currentCommitTime;
	if( mCommittedStreams.empty() == false )
	{
		currentCommitTime = mCommittedStreams.begin()->second.back().mTime;
	}
	RFLOG_TEST_AND_FATAL( currentCommitTime < headTime, nullptr, RFCAT_ROLLBACK, "New stream's head time isn't after the current commit time" );

	InputStream& newCommitted = mCommittedStreams[identifier];
	newCommitted.increase_write_head( currentCommitTime );

	InputStream& newUncommitted = mUncommittedStreams[identifier];
	newUncommitted.increase_write_head( currentCommitTime );
	newUncommitted.increase_read_head( currentCommitTime );
}



InclusiveTimeRange RollbackManager::GetFramesReadyToCommit() const
{
	time::CommonClock::time_point maxCommitHead = time::CommonClock::kLowest;
	if( mCommittedStreams.empty() == false )
	{
		// All committed streams should share the same commit head
		maxCommitHead = mCommittedStreams.begin()->second.back().mTime;
		for( InputStreams::value_type const& entry : mCommittedStreams )
		{
			time::CommonClock::time_point const head = entry.second.back().mTime;
			RFLOG_TEST_AND_FATAL( head == maxCommitHead, nullptr, RFCAT_ROLLBACK, "Committed streams are out of sync" );
		}
	}

	RF_ASSERT( mUncommittedStreams.empty() == false );
	time::CommonClock::time_point minUncommitHead = time::CommonClock::kMax;
	for( InputStreams::value_type const& entry : mUncommittedStreams )
	{
		time::CommonClock::time_point const head = entry.second.back().mTime;
		if( head == maxCommitHead )
		{
			// Should only have an empty frame marker
			RFLOG_TEST_AND_FATAL( entry.second.back().mValue == kInvalidInputValue, nullptr, RFCAT_ROLLBACK, "Uncommitted stream has data at commit head" );
		}
		RFLOG_TEST_AND_FATAL( head >= maxCommitHead, nullptr, RFCAT_ROLLBACK, "Uncommitted stream has stale data" );
		minUncommitHead = rftl::min( minUncommitHead, head );
	}
	RF_ASSERT( maxCommitHead <= minUncommitHead );

	return InclusiveTimeRange( maxCommitHead, minUncommitHead );
}



void RollbackManager::CommitFrames( InclusiveTimeRange const& range, time::CommonClock::time_point const& nextFrame )
{
	RF_ASSERT( range.first <= range.second );
	RF_ASSERT( range.second <= nextFrame );

	// Process each uncommitted stream
	for( InputStreams::value_type& sourceEntry : mUncommittedStreams )
	{
		// Will transfer to committed stream (create if doesn't exist yet)
		InputStream& source = sourceEntry.second;
		InputStream& dest = mCommittedStreams[sourceEntry.first];

		// Transfer
		for( InputStream::const_iterator iter = source.begin(); iter != source.end(); iter++ )
		{
			if( iter->mTime < range.first )
			{
				// Likely a marker from a previous commit
				RF_ASSERT( iter->mValue == kInvalidInputValue );
				continue;
			}

			if( iter->mTime > range.second )
			{
				// Not part of this commit
				break;
			}

			if( iter->mValue == kInvalidInputValue )
			{
				// Blank frame, not interesting
				continue;
			}
			dest.push_back( *iter );
		}

		// Truncate source
		source.discard_old_events( range.second );
	}

	// Lock all transferred frames on the committed streams
	// NOTE: This includes streams that didn't match an uncommitted stream,
	//  which will cause a failure if an uncommitted stream shows up late and
	//  tries to re-commit
	for( InputStreams::value_type& destEntry : mCommittedStreams )
	{
		destEntry.second.increase_write_head( range.second );
	}
}



void RollbackManager::RewindAllStreams( time::CommonClock::time_point time )
{
	for( InputStreams::value_type& entry : mCommittedStreams )
	{
		entry.second.rewind( time );
	}
	for( InputStreams::value_type& entry : mUncommittedStreams )
	{
		entry.second.rewind( time );
	}
}



void RollbackManager::RemoveAllStreams()
{
	mCommittedStreams.clear();
	mUncommittedStreams.clear();
}



void RollbackManager::TrimOldCommittedInputs( time::CommonClock::time_point time )
{
	for( InputStreams::value_type& entry : mCommittedStreams )
	{
		entry.second.increase_read_head( time );
	}
	for( InputStreams::value_type& entry : mUncommittedStreams )
	{
		RFLOG_TEST_AND_FATAL(
			entry.second.front().mTime > time,
			nullptr,
			RFCAT_ROLLBACK,
			"Trimming committed streams conflicts with uncommitted events before trim point" );
	}
}



size_t RollbackManager::ClearAllSnapshotsBefore( time::CommonClock::time_point time )
{
	size_t retVal = 0;
	retVal += mSharedDomain.ClearAllSnapshotsBefore( time );
	retVal += mPrivateDomain.ClearAllSnapshotsBefore( time );
	return retVal;
}



size_t RollbackManager::ClearAllSnapshotsAfter( time::CommonClock::time_point time )
{
	size_t retVal = 0;
	retVal += mSharedDomain.ClearAllSnapshotsAfter( time );
	retVal += mPrivateDomain.ClearAllSnapshotsAfter( time );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

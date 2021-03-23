#pragma once
#include "StateTree.h"

#include "core_allocate/DefaultAllocCreator.h"
#include "core_math/math_clamps.h"


namespace RF::state {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueT, size_t MaxChangesT>
WeakPtr<typename StateTree<ValueT, MaxChangesT>::StreamType> StateTree<ValueT, MaxChangesT>::GetOrCreateStream( VariableIdentifier const& identifier, alloc::Allocator& allocator )
{
	// Initial lock assumes we can return without mutation
	{
		ReaderLock lock( mMultiReaderSingleWriterLock );

		typename Tree::const_iterator const iter = mTree.find( identifier );
		if( iter != mTree.end() )
		{
			return iter->second;
		}
	}

	// Elevate to writer lock and try again
	{
		WriterLock lock( mMultiReaderSingleWriterLock );

		// NOTE: Have to re-search since we briefly dropped the lock
		typename Tree::const_iterator const iter = mTree.find( identifier );
		if( iter == mTree.end() )
		{
			UniquePtr<StreamType> newStream = alloc::DefaultAllocCreator<StreamType>::Create( allocator );
			typename Tree::iterator const newIter = mTree.emplace( identifier, rftl::move( newStream ) ).first;
			WeakPtr<StreamType> const retVal = newIter->second;
			return retVal;
		}
		return iter->second;
	}
}



template<typename ValueT, size_t MaxChangesT>
inline UniquePtr<typename StateTree<ValueT, MaxChangesT>::StreamType> StateTree<ValueT, MaxChangesT>::RemoveStream( VariableIdentifier const& identifier )
{
	WriterLock lock( mMultiReaderSingleWriterLock );

	typename Tree::iterator const iter = mTree.find( identifier );
	if( iter == mTree.end() )
	{
		return nullptr;
	}
	UniquePtr<StreamType> retVal = rftl::move( iter->second );
	mTree.erase( iter );
	return retVal;
}



template<typename ValueT, size_t MaxChangesT>
inline size_t StateTree<ValueT, MaxChangesT>::RemoveAllStreams()
{
	WriterLock lock( mMultiReaderSingleWriterLock );

	size_t const retVal = mTree.size();
	mTree.clear();
	return retVal;
}



template<typename ValueT, size_t MaxChangesT>
WeakPtr<typename StateTree<ValueT, MaxChangesT>::StreamType const> StateTree<ValueT, MaxChangesT>::GetStream( VariableIdentifier const& identifier ) const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	typename Tree::const_iterator const iter = mTree.find( identifier );
	if( iter == mTree.end() )
	{
		return nullptr;
	}
	return iter->second;
}



template<typename ValueT, size_t MaxChangesT>
WeakPtr<typename StateTree<ValueT, MaxChangesT>::StreamType> StateTree<ValueT, MaxChangesT>::GetMutableStream( VariableIdentifier const& identifier )
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	typename Tree::const_iterator const iter = mTree.find( identifier );
	if( iter == mTree.end() )
	{
		return nullptr;
	}
	return iter->second;
}



template<typename ValueT, size_t MaxChangesT>
inline size_t StateTree<ValueT, MaxChangesT>::RewindAllStreams( time::CommonClock::time_point time )
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	// Discarding everything after the given time will effectively be a rewind
	//  to that time
	time::CommonClock::time_point const startDiscardingAt = time + time::CommonClock::duration( 1 );

	for( typename Tree::value_type const& entry : mTree )
	{
		entry.second->Discard( startDiscardingAt );
	}

	return mTree.size();
}



template<typename ValueT, size_t MaxChangesT>
inline rftl::optional<InclusiveTimeRange> StateTree<ValueT, MaxChangesT>::GetEarliestTimes() const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	if( mTree.empty() )
	{
		return rftl::optional<InclusiveTimeRange>();
	}

	InclusiveTimeRange range = { time::CommonClock::kMax, time::CommonClock::kLowest };
	for( typename Tree::value_type const& entry : mTree )
	{
		time::CommonClock::time_point const time = entry.second->GetEarliestTime();
		range.first = math::Min( range.first, time );
		range.second = math::Max( range.second, time );
	}
	return range;
}



template<typename ValueT, size_t MaxChangesT>
inline rftl::optional<InclusiveTimeRange> StateTree<ValueT, MaxChangesT>::GetLatestTimes() const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	if( mTree.empty() )
	{
		return rftl::optional<InclusiveTimeRange>();
	}

	InclusiveTimeRange range = { time::CommonClock::kMax, time::CommonClock::kLowest };
	for( typename Tree::value_type const& entry : mTree )
	{
		time::CommonClock::time_point const time = entry.second->GetLatestTime();
		range.first = math::Min( range.first, time );
		range.second = math::Max( range.second, time );
	}
	RF_ASSERT( range.first <= range.second );
	return range;
}



template<typename ValueT, size_t MaxChangesT>
inline rftl::vector<VariableIdentifier> StateTree<ValueT, MaxChangesT>::GetIdentifiers() const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	rftl::vector<VariableIdentifier> retVal;
	retVal.reserve( mTree.size() );
	for( typename Tree::value_type const& entry : mTree )
	{
		retVal.emplace_back( entry.first );
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

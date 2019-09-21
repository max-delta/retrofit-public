#pragma once
#include "StateStream.h"

#include "rftl/algorithm"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueT, size_t MaxChangesT>
inline StateStream<ValueT, MaxChangesT>::StateStream()
{
	//
}



template<typename ValueT, size_t MaxChangesT>
inline void StateStream<ValueT, MaxChangesT>::Write( time::CommonClock::time_point time, ValueT value )
{
	ChangeType newChange;
	newChange.mTime = time;
	newChange.mNewValue = value;

	static constexpr auto compare = [](
		time::CommonClock::time_point const& time,
		ChangeType const& change ) -> bool
	{
		return time < change.mTime;
	};

	typename Changes::iterator iterPastTime = rftl::upper_bound( mChanges.begin(), mChanges.end(), time, compare );
	if( iterPastTime == mChanges.end() )
	{
		// Need to add to end

		if( mChanges.size() == mChanges.max_size() )
		{
			// Full, pop front
			mChanges.erase( mChanges.begin() );
		}

		mChanges.emplace_back( newChange );
		return;
	}

	// Need to stomp and wipe everything aftewards
	*iterPastTime = newChange;

	RF_ASSERT( mChanges.begin() <= iterPastTime );
	size_t const newSize = static_cast<size_t>( rftl::distance( mChanges.begin(), iterPastTime + 1 ) );
	mChanges.resize( newSize );
}



template<typename ValueT, size_t MaxChangesT>
inline ValueT StateStream<ValueT, MaxChangesT>::Read( time::CommonClock::time_point time ) const
{
	static constexpr auto compare = [](
		time::CommonClock::time_point const& time,
		ChangeType const& change ) -> bool
	{
		return time < change.mTime;
	};

	typename Changes::const_iterator iterPastTime = rftl::upper_bound( mChanges.begin(), mChanges.end(), time, compare );
	if( iterPastTime == mChanges.begin() )
	{
		return ValueType{};
	}
	return ( iterPastTime - 1 )->mNewValue;
}



template<typename ValueT, size_t MaxChangesT>
inline time::CommonClock::time_point StateStream<ValueT, MaxChangesT>::GetEarliestTime() const
{
	if( mChanges.empty() )
	{
		return time::CommonClock::time_point();
	}
	return mChanges.front().mTime;
}



template<typename ValueT, size_t MaxChangesT>
inline time::CommonClock::time_point StateStream<ValueT, MaxChangesT>::GetLatestTime() const
{
	if( mChanges.empty() )
	{
		return time::CommonClock::time_point();
	}
	return mChanges.back().mTime;
}

///////////////////////////////////////////////////////////////////////////////
}}

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
	RF_ASSERT( mTimes.size() == mValues.size() );

	typename Times::iterator iterPastTime = rftl::upper_bound( mTimes.begin(), mTimes.end(), time );
	if( iterPastTime == mTimes.end() )
	{
		// Need to add to end

		if( mTimes.size() == mTimes.max_size() )
		{
			// Full, pop front
			mTimes.erase( mTimes.begin() );
			mTimes.erase( mTimes.begin() );
		}

		mTimes.emplace_back( time );
		mValues.emplace_back( value );
		return;
	}

	typename Times::difference_type const distance = iterPastTime - mTimes.begin();
	typename Values::iterator const iterPastValue = mValues.begin() + distance;

	// Need to stomp and wipe everything aftewards
	*iterPastTime = time;
	*iterPastValue = value;

	RF_ASSERT( mTimes.begin() <= iterPastTime );
	RF_ASSERT( mValues.begin() <= iterPastValue );
	size_t const newSize = static_cast<size_t>( rftl::distance( mTimes.begin(), iterPastTime + 1 ) );
	mTimes.resize( newSize );
	mValues.resize( newSize );
}



template<typename ValueT, size_t MaxChangesT>
inline ValueT StateStream<ValueT, MaxChangesT>::Read( time::CommonClock::time_point time ) const
{
	RF_ASSERT( mTimes.size() == mValues.size() );

	typename Times::const_iterator iterPastTime = rftl::upper_bound( mTimes.begin(), mTimes.end(), time );
	if( iterPastTime == mTimes.begin() )
	{
		return ValueType{};
	}

	typename Times::difference_type const distance = iterPastTime - mTimes.begin();
	typename Values::const_iterator const iterPastValue = mValues.begin() + distance;

	return *( iterPastValue - 1 );
}



template<typename ValueT, size_t MaxChangesT>
inline time::CommonClock::time_point StateStream<ValueT, MaxChangesT>::GetEarliestTime() const
{
	if( mTimes.empty() )
	{
		return time::CommonClock::time_point();
	}
	return mTimes.front();
}



template<typename ValueT, size_t MaxChangesT>
inline time::CommonClock::time_point StateStream<ValueT, MaxChangesT>::GetLatestTime() const
{
	if( mTimes.empty() )
	{
		return time::CommonClock::time_point();
	}
	return mTimes.back();
}

///////////////////////////////////////////////////////////////////////////////
}}

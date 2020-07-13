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

	if( value == Read( time ) )
	{
		// Writing this value wouldn't change the read behavior, so we'll skip
		//  it to keep the stream high-entropy for performance reasons
		return;
	}

	typename Times::iterator iterAtTime = rftl::lower_bound( mTimes.begin(), mTimes.end(), time );
	if( iterAtTime == mTimes.end() )
	{
		// Need to add to end

		if( mTimes.size() == mTimes.max_size() )
		{
			// Full, pop front
			mTimes.erase( mTimes.begin() );
			mValues.erase( mValues.begin() );
		}

		mTimes.emplace_back( time );
		mValues.emplace_back( value );
		return;
	}

	typename Times::difference_type const distance = iterAtTime - mTimes.begin();
	typename Values::iterator const iterAtValue = mValues.begin() + distance;

	// Need to stomp and wipe everything aftewards
	*iterAtTime = time;
	*iterAtValue = value;

	RF_ASSERT( mTimes.begin() <= iterAtTime );
	RF_ASSERT( mValues.begin() <= iterAtValue );
	size_t const newSize = static_cast<size_t>( rftl::distance( mTimes.begin(), iterAtTime + 1 ) );
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
inline void StateStream<ValueT, MaxChangesT>::Discard( time::CommonClock::time_point time )
{
	RF_ASSERT( mTimes.size() == mValues.size() );

	typename Times::iterator iterAtTime = rftl::lower_bound( mTimes.begin(), mTimes.end(), time );
	if( iterAtTime == mTimes.end() )
	{
		// Nothing to discard
		return;
	}

	typename Times::difference_type const distance = iterAtTime - mTimes.begin();
	typename Values::iterator const iterAtValue = mValues.begin() + distance;

	// Need to stomp and wipe everything at and aftewards
	RF_ASSERT( mTimes.begin() <= iterAtTime );
	RF_ASSERT( mValues.begin() <= iterAtValue );
	size_t const newSize = static_cast<size_t>( rftl::distance( mTimes.begin(), iterAtTime ) );
	mTimes.resize( newSize );
	mValues.resize( newSize );
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

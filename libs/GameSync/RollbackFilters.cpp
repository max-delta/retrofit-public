#include "stdafx.h"
#include "RollbackFilters.h"

#include "Rollback/RollbackManager.h"
#include "Rollback/InputStreamRef.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

rollback::InputStreamRef RollbackFilters::GetMutableStreamRef(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamIdentifier const& identifier )
{
	rollback::InputStream& uncommitted = rollMan.GetMutableUncommittedStreams().at( identifier );
	rollback::InputStream const& committed = rollMan.GetCommittedStreams().at( identifier );
	return rollback::InputStreamRef( identifier, uncommitted, committed );
}



bool RollbackFilters::CanOverlapOrSuffix(
	rollback::RollbackManager const& rollMan,
	rollback::InputStreamRef const& streamRef,
	rftl::vector<rollback::InputEvent> const& newEvents,
	size_t& overlapCount )
{
	using rollback::InputStream;
	using rollback::InputEvent;

	RF_ASSERT( streamRef.mCommitted.back().mTime <= streamRef.mUncommitted.front().mTime );
	overlapCount = 0;

	if( newEvents.empty() )
	{
		// Trivial, no events
		overlapCount = 0;
		return true;
	}

	static constexpr auto timeCompare = []( InputEvent const& lhs, InputEvent const& rhs ) -> bool //
	{
		return lhs.mTime < rhs.mTime;
	};
	RF_ASSERT( rftl::is_sorted( newEvents.begin(), newEvents.end(), timeCompare ) );

	if( streamRef.mUncommitted.back().mTime < newEvents.front().mTime )
	{
		// Trivial, all after
		overlapCount = 0;
		return true;
	}

	if( newEvents.back().mTime < streamRef.mCommitted.front().mTime )
	{
		// Trivial, all before
		overlapCount = 0;
		return false;
	}

	// Need to they merge at the end cleanly
	time::CommonClock::time_point const mergeTime = streamRef.mUncommitted.back().mTime;

	// Try to find the merge point, with maybe a bit extra
	using ReverseVecIter = rftl::vector<InputEvent>::const_reverse_iterator;
	ReverseVecIter newRBegin = newEvents.rbegin();
	ReverseVecIter const newREnd = newEvents.rend();
	while( newRBegin != newREnd && mergeTime < newRBegin->mTime )
	{
		// Anything after merge time is trivially safe
		newRBegin++;
	}
	RF_ASSERT( newRBegin != newREnd );

	// Copy old streams into a single buffer for sanity
	size_t const maxSearchSpace = math::integer_cast<size_t>( rftl::distance( newRBegin, newREnd ) );
	rftl::vector<InputEvent> oldEvents;
	oldEvents.reserve( maxSearchSpace );
	static constexpr auto fill = []( rftl::vector<InputEvent>& output, InputStream const& stream, size_t maxSize ) -> void //
	{
		for( InputStream::const_reverse_iterator iter = stream.rbegin(); iter != stream.rend(); iter++ )
		{
			if( output.size() == maxSize )
			{
				break;
			}
			if( iter->mValue != rollback::kInvalidInputValue )
			{
				output.insert( output.begin(), *iter );
			}
		}
	};
	fill( oldEvents, streamRef.mUncommitted, maxSearchSpace );
	fill( oldEvents, streamRef.mCommitted, maxSearchSpace );
	RF_ASSERT( oldEvents.size() <= maxSearchSpace );
	if( oldEvents.empty() )
	{
		// Special case, same frame as write head, but no events to try and
		//  match against, should be an empty stream
		RF_ASSERT( streamRef.mCommitted.empty() );
		RF_ASSERT( streamRef.mUncommitted.empty() );
		RF_ASSERT( streamRef.mUncommitted.back().mTime == newEvents.front().mTime );
		overlapCount = 0;
		return true;
	}
	ReverseVecIter const oldRBegin = oldEvents.rbegin();
	ReverseVecIter const oldREnd = oldEvents.rend();

	// Try to search for the merge point that overlaps cleanly
	while( true )
	{
		if( newRBegin == newREnd )
		{
			// Could not find a match, end of list
			overlapCount = 0;
			return false;
		}

		if( newRBegin->mTime < oldRBegin->mTime )
		{
			// Could not find a match, crossed over search time
			overlapCount = 0;
			return false;
		}

		ReverseVecIter const oldRIterMatch = rftl::search(
			oldRBegin, oldREnd,
			newRBegin, newREnd,
			InputEvent::AreEqual );

		if( oldRIterMatch == oldREnd )
		{
			// No match, trim the new events and try again
			newRBegin++;
			continue;
		}

		if( oldRIterMatch != oldRBegin )
		{
			// Found a match, but it's not overlapping the end (which is RBegin)
			overlapCount = 0;
			return false;
		}

		size_t const successfulMatchSize = math::integer_cast<size_t>( rftl::distance( newRBegin, newREnd ) );
		overlapCount = successfulMatchSize;
		return true;
	}
}



void RollbackFilters::PrepareLocalFrame(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamRef const& streamRef,
	time::CommonClock::time_point frameTime )
{
	streamRef.mUncommitted.increase_write_head( frameTime );
}



bool RollbackFilters::TryPrepareRemoteFrame(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamRef const& streamRef,
	time::CommonClock::time_point frameTime )
{
	using rollback::RollbackManager;

	if( frameTime < streamRef.mUncommitted.back().mTime )
	{
		RFLOG_WARNING(
			nullptr,
			RFCAT_GAMESYNC,
			"Failed to prepare a remote frame for input stream '%u' because it"
			" was too far in the past",
			streamRef.mIdentifier );
		return false;
	}

	if( frameTime < streamRef.mCommitted.back().mTime )
	{
		RFLOG_WARNING(
			nullptr,
			RFCAT_GAMESYNC,
			"Failed to prepare a remote frame for input stream '%u' because it"
			" would conflict with already committed data",
			streamRef.mIdentifier );
		return false;
	}

	streamRef.mUncommitted.increase_write_head( frameTime );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

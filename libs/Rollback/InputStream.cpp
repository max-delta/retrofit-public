#include "stdafx.h"
#include "InputStream.h"

#include "Logging/Logging.h"

#include "rftl/algorithm"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

InputStream::value_type const& InputStream::at( size_t index ) const
{
	return Parent::at( index );
}



InputStream::value_type const& InputStream::operator[]( size_t index ) const
{
	return Parent::operator[]( index );
}



InputStream::value_type InputStream::front() const
{
	if( empty() )
	{
		// No events
		return value_type{ mWriteHead, kInvalidInputValue };
	}

	return Parent::front();
}



InputStream::value_type InputStream::back() const
{
	if( empty() )
	{
		// No events
		return value_type{ mWriteHead, kInvalidInputValue };
	}

	value_type const& back = Parent::back();
	if( back.mTime != mWriteHead )
	{
		RFLOG_TEST_AND_FATAL( back.mTime <= mWriteHead, nullptr, RFCAT_ROLLBACK, "Head is before back" );

		// Head is after back
		return value_type{ mWriteHead, kInvalidInputValue };
	}
	else
	{
		// Head is at back
		return back;
	}
}



InputStream::const_iterator InputStream::begin() const
{
	return cbegin();
}



InputStream::const_iterator InputStream::end() const
{
	return cend();
}



InputStream::const_reverse_iterator InputStream::rbegin() const
{
	return crbegin();
}



InputStream::const_reverse_iterator InputStream::rend() const
{
	return crend();
}



void InputStream::reset()
{
	clear();
	mWriteHead = {};
}



void InputStream::push_back( value_type const& value )
{
	push_back( value_type( value ) );
}



void InputStream::push_back( value_type&& value )
{
	RFLOG_TEST_AND_FATAL( value.mValue != kInvalidInputValue, nullptr, RFCAT_ROLLBACK, "Trying to insert an invalid event value" );

	RFLOG_TEST_AND_FATAL( mWriteHead <= value.mTime, nullptr, RFCAT_ROLLBACK, "Trying to insert an out-of-order event" );
	mWriteHead = value.mTime;

	Parent::push_back( rftl::move( value ) );
}



void InputStream::increase_write_head( time::CommonClock::time_point time )
{
	RFLOG_TEST_AND_FATAL( mWriteHead <= time, nullptr, RFCAT_ROLLBACK, "Trying to move write-head bacwards" );
	mWriteHead = time;
}



void InputStream::increase_read_head( time::CommonClock::time_point time )
{
	RFLOG_TEST_AND_FATAL( time <= mWriteHead, nullptr, RFCAT_ROLLBACK, "Trying to move read-head past write-head" );

	if( empty() )
	{
		// Trivial
		Parent::emplace_back( value_type( time, kInvalidInputValue ) );
		return;
	}

	// Truncate
	const_iterator const iter = lower_bound( time );
	erase( cbegin(), iter );
	if( empty() )
	{
		Parent::emplace_back( value_type( time, kInvalidInputValue ) );
	}
}



void InputStream::rewind( time::CommonClock::time_point time )
{
	RFLOG_TEST_AND_FATAL( time <= mWriteHead, nullptr, RFCAT_ROLLBACK, "Trying to rewind write-head forward" );

	if( time == mWriteHead )
	{
		// Trivial
		return;
	}

	if( empty() )
	{
		// Trivial
		mWriteHead = time;
		return;
	}

	if( time <= front().mTime )
	{
		// Trivial, full truncate
		clear();
		mWriteHead = time;
		Parent::emplace_back( value_type( time, kInvalidInputValue ) );
		return;
	}

	// Truncate
	const_iterator const iter = lower_bound( time );
	erase( iter, cend() );
	mWriteHead = time;
	if( empty() )
	{
		Parent::emplace_back( value_type( time, kInvalidInputValue ) );
	}
}



void InputStream::discard_old_events( time::CommonClock::time_point inclusiveTime )
{
	if( empty() )
	{
		// Trivial
		return;
	}

	// Truncate
	const_iterator const iter = upper_bound( inclusiveTime );
	erase( cbegin(), iter );
}



InputStream::const_iterator InputStream::lower_bound( time::CommonClock::time_point time ) const
{
	static constexpr auto comp = []( value_type const& lhs, time::CommonClock::time_point const& rhs ) -> bool {
		return lhs.mTime < rhs;
	};
	return rftl::lower_bound( cbegin(), cend(), time, comp );
}



InputStream::const_iterator InputStream::upper_bound( time::CommonClock::time_point time ) const
{
	static constexpr auto comp = []( time::CommonClock::time_point const& lhs, value_type const& rhs ) -> bool {
		return lhs < rhs.mTime;
	};
	return rftl::upper_bound( cbegin(), cend(), time, comp );
}

///////////////////////////////////////////////////////////////////////////////
}

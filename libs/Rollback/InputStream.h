#pragma once
#include "project.h"

#include "Rollback/RollbackFwd.h"

#include "core/macros.h"

#include "rftl/deque"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

struct ROLLBACK_API InputEvent
{
	InputEvent() = default;
	InputEvent( time::CommonClock::time_point time, InputValue value );

	time::CommonClock::time_point mTime;
	InputValue mValue;
};

///////////////////////////////////////////////////////////////////////////////

// Input streams are ordered by increasing time and have limited mutability
class ROLLBACK_API InputStream final : private rftl::deque<InputEvent>
{
	RF_NO_COPY( InputStream );
	RF_NO_MOVE( InputStream );

	//
	// Types and constants
private:
	using Parent = rftl::deque<InputEvent>;

public:
	using Parent::value_type;

	using Parent::iterator;
	using Parent::const_iterator;
	using Parent::reverse_iterator;
	using Parent::const_reverse_iterator;


	//
	// Public methods
public:
	InputStream() = default;
	~InputStream() = default;

	value_type const& at( size_t index ) const;
	value_type const& operator[]( size_t index ) const;

	// NOTE: Return-by-value, these will always succeed, but may return frames
	//  with no input, indicated by an invalid event value but a valid time
	value_type front() const;
	value_type back() const;

	using Parent::cbegin;
	using Parent::cend;
	using Parent::crbegin;
	using Parent::crend;
	const_iterator begin() const;
	const_iterator end() const;
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;

	using Parent::empty;
	using Parent::size;
	using Parent::max_size;

	void reset();

	void push_back( value_type const& value );
	void push_back( value_type&& value );

	template<typename... ArgsT>
	void emplace_back( ArgsT&&... args )
	{
		push_back( rftl::move( value_type( args... ) ) );
	}

	// Move forward the leading write head, without actually writing any events
	void increase_write_head( time::CommonClock::time_point time );

	// Move forward the trailing read head, likely truncating events
	void increase_read_head( time::CommonClock::time_point time );

	// Move backward the write head, likely truncating events
	void rewind( time::CommonClock::time_point time );

	// Points to the start of the frame
	// NOTE: Frame may have 0 elements, and so this may point to the next frame
	const_iterator lower_bound( time::CommonClock::time_point time ) const;

	// Points to the start of the following frame
	const_iterator upper_bound( time::CommonClock::time_point time ) const;


	//
	// Private data
private:
	time::CommonClock::time_point mWriteHead = {};
};

///////////////////////////////////////////////////////////////////////////////
}}

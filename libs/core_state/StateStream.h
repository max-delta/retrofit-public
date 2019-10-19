#pragma once
#include "StateFwd.h"

#include "core_time/CommonClock.h"

#include "rftl/extension/static_vector.h"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

// A state stream represents a variable with limited historical knowledge of
//  past values, primarily intended for rollback-based collision resolution
//  where time may need to be rewinded within a very short window
template<typename ValueT, size_t MaxChangesT>
class StateStream
{
	//
	// Types and constants
public:
	using TimeType = time::CommonClock::time_point;
	using ValueType = ValueT;
	static constexpr size_t kMaxChanges = MaxChangesT;

private:
	// TODO: These would likely be better served by circular buffers
	using Times = rftl::static_vector<TimeType, kMaxChanges>;
	using Values = rftl::static_vector<ValueType, kMaxChanges>;


	//
	// Public methods
public:
	StateStream();

	void Write( time::CommonClock::time_point time, ValueT value );
	ValueT Read( time::CommonClock::time_point time ) const;

	time::CommonClock::time_point GetEarliestTime() const;
	time::CommonClock::time_point GetLatestTime() const;


	//
	// Private data
private:
	// NOTE: These are kept seperate to avoid excessive padding from alignment
	Times mTimes;
	Values mValues;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "StateStream.inl"

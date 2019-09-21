#pragma once
#include "StateFwd.h"

#include "core_state/StateChange.h"

#include "rftl/extension/static_array.h"


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
	using ValueType = ValueT;
	using ChangeType = StateChange<ValueType>;
	static constexpr size_t kMaxChanges = MaxChangesT;

private:
	// TODO: This would likely be better served by a circular buffer
	using Changes = rftl::static_array<ChangeType, kMaxChanges>;


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
	Changes mChanges;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "StateStream.inl"

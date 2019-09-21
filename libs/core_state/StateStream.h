#pragma once
#include "StateFwd.h"

#include "core_state/StateChange.h"

#include "rftl/extension/static_array.h"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

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
	using Changes = rftl::static_array<ChangeType, kMaxChanges>;


	//
	// Public methods
public:
	StateStream();

	void Write( time::CommonClock::time_point time, ValueT value );
	ValueT Read( time::CommonClock::time_point time ) const;


	//
	// Private data
private:
	Changes mChanges;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "StateStream.inl"

#pragma once
#include "StateFwd.h"

#include "core_time/CommonClock.h"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueT>
struct StateChange
{
	time::CommonClock::time_point mTime;

	using ValueType = ValueT;
	ValueType mNewValue;
};

///////////////////////////////////////////////////////////////////////////////
}}

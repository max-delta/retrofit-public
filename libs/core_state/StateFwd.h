#pragma once
#include "core_time/CommonClock.h"

#include "rftl/string"

namespace RF::state {
///////////////////////////////////////////////////////////////////////////////

class VariableIdentifier;

template<typename ValueT, size_t MaxChangesT>
class StateStream;

template<size_t MaxChangesT>
class StateBag;

using InclusiveTimeRange = rftl::pair<
	time::CommonClock::time_point,
	time::CommonClock::time_point>;

///////////////////////////////////////////////////////////////////////////////
}

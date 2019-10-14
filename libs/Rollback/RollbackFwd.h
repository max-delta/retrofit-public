#pragma once
#include "core_state/StateFwd.h"
#include "core_time/CommonClock.h"

#include "core/ptr/ptr_fwd.h"

#include "rftl/utility"

namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

class RollbackManager;
class Domain;

static constexpr size_t kMaxChangesInWindow = 10;
using Window = state::StateBag<kMaxChangesInWindow>;
template<typename T>
using Stream = state::StateStream<T, kMaxChangesInWindow>;
template<typename T>
class Var;

using Snapshot = rftl::pair<time::CommonClock::time_point, state::StateBag<1>>;
using ManualSnapshotIdentifier = rftl::string;

///////////////////////////////////////////////////////////////////////////////
}}

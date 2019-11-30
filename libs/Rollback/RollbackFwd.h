#pragma once
#include "core_state/StateFwd.h"
#include "core_time/CommonClock.h"

#include "core/ptr/ptr_fwd.h"

#include "rftl/utility"

namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

class RollbackManager;
class Domain;

// Expected to be used for 'Player 1...N'
// NOTE: Zero is a valid identifier, and is expected (but not assumed) to be
//  used for private input, such as local-only menu navigation or any other
//  input that only affects private state
using InputStreamIdentifier = uint8_t;

using InputValue = uint64_t;
static constexpr InputValue kInvalidInputValue = 0;

struct InputEvent;
class InputStream;

struct InputStreamRef;
struct InputStreamConstRef;

static constexpr size_t kMaxChangesInWindow = 10;
using Window = state::StateBag<kMaxChangesInWindow>;
template<typename T>
using Stream = state::StateStream<T, kMaxChangesInWindow>;
template<typename T>
class Var;
template<typename T>
class AutoVar;

using Snapshot = rftl::pair<
	time::CommonClock::time_point,
	state::StateBag<1>>;
using ManualSnapshotIdentifier = rftl::string;

using InclusiveTimeRange = rftl::pair<
	time::CommonClock::time_point,
	time::CommonClock::time_point>;

///////////////////////////////////////////////////////////////////////////////
}}

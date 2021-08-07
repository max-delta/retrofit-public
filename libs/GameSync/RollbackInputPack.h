#pragma once
#include "project.h"

#include "Rollback/InputEvent.h"

#include "rftl/vector"
#include "rftl/unordered_map"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

// Combined data for sending updates about uncommitted input streams
struct GAMESYNC_API RollbackInputPack final
{
	static constexpr size_t kMaxEventsPerStream = rftl::numeric_limits<uint8_t>::max();
	using Events = rftl::vector<rollback::InputEvent>;

	// NOTE: A common case is that streams will be present, but empty, as there
	//  have been no recent uncommitted input events
	static constexpr size_t kMaxStreams = rftl::numeric_limits<uint8_t>::max();
	using Streams = rftl::unordered_map<rollback::InputStreamIdentifier, Events>;

	// All inputs must be before this, and no changes will happen to inputs on
	//  this frame, or any frame preceding it
	time::CommonClock::time_point mFrameReadyToCommit = {};

	Streams mStreams;
};

///////////////////////////////////////////////////////////////////////////////
}

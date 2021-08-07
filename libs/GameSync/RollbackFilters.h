#pragma once
#include "project.h"

#include "Rollback/RollbackFwd.h"

#include "core/macros.h"

#include "rftl/vector"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

class GAMESYNC_API RollbackFilters final
{
	RF_NO_INSTANCE( RollbackFilters );

public:
	static rollback::InputStreamRef GetMutableStreamRef(
		rollback::RollbackManager& rollMan,
		rollback::InputStreamIdentifier const& identifier );

	static bool CanOverlapOrSuffix(
		rollback::RollbackManager const& rollMan,
		rollback::InputStreamRef const& streamRef,
		rftl::vector<rollback::InputEvent> const& newEvents,
		size_t& overlapCount );

	// Prepare a frame for writing
	// NOTE: Trusted local input sources must succeed
	// NOTE: Untrusted remote input sources may fail
	static void PrepareLocalFrame(
		rollback::RollbackManager& rollMan,
		rollback::InputStreamRef const& streamRef,
		time::CommonClock::time_point frameTime );
	static bool TryPrepareRemoteFrame(
		rollback::RollbackManager& rollMan,
		rollback::InputStreamRef const& streamRef,
		time::CommonClock::time_point frameTime );
};

///////////////////////////////////////////////////////////////////////////////
}

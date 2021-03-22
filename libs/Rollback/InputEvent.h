#pragma once
#include "project.h"

#include "Rollback/RollbackFwd.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

struct ROLLBACK_API InputEvent
{
	InputEvent() = default;
	InputEvent( time::CommonClock::time_point time, InputValue value );

	static bool AreEqual( InputEvent const& lhs, InputEvent const& rhs );

	time::CommonClock::time_point mTime;
	InputValue mValue;
};

///////////////////////////////////////////////////////////////////////////////
}

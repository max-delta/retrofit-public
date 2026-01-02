#pragma once
#include "project.h"


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

struct StartupConfig
{
	bool mInformativeLogging = config::flag::kInformativeLogging;
	bool mVerboseLogging = config::flag::kVerboseLogging;
	bool mTraceLogging = config::flag::kTraceLogging;
};

///////////////////////////////////////////////////////////////////////////////
}

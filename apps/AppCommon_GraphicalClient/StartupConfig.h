#pragma once
#include "project.h"


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

struct StartupConfig
{
	bool mInformativeLogging = config::kInformativeLogging;
	bool mVerboseLogging = config::kVerboseLogging;
	bool mTraceLogging = config::kTraceLogging;
};

///////////////////////////////////////////////////////////////////////////////
}

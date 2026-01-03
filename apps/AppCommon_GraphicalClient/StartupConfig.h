#pragma once
#include "project.h"

#include "CommandLine/CommandLineFwd.h"


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

struct StartupConfig
{
	void AdjustFromArgs( cli::ArgParse const& args );

	bool mInformativeLogging = config::flag::kInformativeLogging;
	bool mVerboseLogging = config::flag::kVerboseLogging;
	bool mTraceLogging = config::flag::kTraceLogging;
};

///////////////////////////////////////////////////////////////////////////////
}

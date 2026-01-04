#pragma once
#include "project.h"

#include "CommandLine/CommandLineFwd.h"

#include "rftl/string"


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

struct StartupConfig
{
	void AdjustFromArgs( cli::ArgParse const& args );

	rftl::string mWindowTitle;
	bool mInformativeLogging = config::flag::kInformativeLogging;
	bool mVerboseLogging = config::flag::kVerboseLogging;
	bool mTraceLogging = config::flag::kTraceLogging;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once

#include "CommandLine/ArgView.h"


namespace RF::bindump {
///////////////////////////////////////////////////////////////////////////////

enum class ErrorReturnCode : int
{
	Success = 0,

	UnknownError = 1,

	// Asked for help prompt, version info, etc
	NonActionableArg,

	// Missing required args, etc
	BadArgUsage,

	// Can't access a file
	FileAccessError,
};

ErrorReturnCode Init( cli::ArgView const& args );
ErrorReturnCode Process();

///////////////////////////////////////////////////////////////////////////////
}

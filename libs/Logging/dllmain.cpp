#include "stdafx.h"
#include "core_platform/dllmain.h"

// Direct forwards to avoid Windows.h contamination into headers
namespace RF { namespace logging {
void InsertFallbackLogger();
}}

shim::BOOL OnDllMainProcessAttach()
{
	RF::logging::InsertFallbackLogger();
	return 1 /*win32::TRUE*/;
}

shim::BOOL OnDllMainProcessDetach()
{
	return 1 /*win32::TRUE*/;
}

shim::BOOL OnDllMainThreadAttach()
{
	return 1 /*win32::TRUE*/;
}

shim::BOOL OnDllMainThreadDetach()
{
	return 1 /*win32::TRUE*/;
}

RF_DLLMAIN();

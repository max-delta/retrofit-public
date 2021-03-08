#include "stdafx.h"
#include "core_platform/dllmain.h"

// Direct forwards to avoid Windows.h contamination into headers
namespace RF::logging {
void InsertFallbackLogger();
}

shim::BOOL OnDllMainProcessAttach()
{
	RF::logging::InsertFallbackLogger();
	return shim::kTRUE;
}

shim::BOOL OnDllMainProcessDetach()
{
	return shim::kTRUE;
}

shim::BOOL OnDllMainThreadAttach()
{
	return shim::kTRUE;
}

shim::BOOL OnDllMainThreadDetach()
{
	return shim::kTRUE;
}

RF_DLLMAIN();

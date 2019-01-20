#include "stdafx.h"

#include "core_platform/windows_inc.h"


// Direct forwards to avoid Windows.h contamination into headers
namespace RF { namespace logging {
	void InsertFallbackLogger();
}}

win32::BOOL APIENTRY DllMain( win32::HMODULE hModule,
	win32::DWORD ul_reason_for_call,
	win32::LPVOID lpReserved )
{
	switch( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
			RF::logging::InsertFallbackLogger();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
		default:
			break;
	}
	return win32::TRUE;
}

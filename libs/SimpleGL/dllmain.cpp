#include "stdafx.h"

#include "core_platform/windows_inc.h"

win32::BOOL APIENTRY DllMain( win32::HMODULE hModule,
	win32::DWORD  ul_reason_for_call,
	win32::LPVOID lpReserved )
{
	switch( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return win32::TRUE;
}


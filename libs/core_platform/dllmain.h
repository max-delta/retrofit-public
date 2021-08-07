#pragma once
#include "core/compiler.h"

#ifdef RF_PLATFORM_WINDOWS

#include "core_platform/shim/win_shim.h"
#define RF_EMPTY_DLLMAIN() \
	shim::BOOL __stdcall DllMain( shim::HMODULE hModule, \
		shim::DWORD ul_reason_for_call, \
		shim::LPVOID lpReserved ) \
	{ \
		return shim::kTRUE; \
	}
#define RF_DLLMAIN() \
	shim::BOOL __stdcall DllMain( shim::HMODULE hModule, \
		shim::DWORD ul_reason_for_call, \
		shim::LPVOID lpReserved ) \
	{ \
		switch( ul_reason_for_call ) \
		{ \
			case 1 /*DLL_PROCESS_ATTACH*/: \
				return OnDllMainProcessAttach(); \
			case 2 /*DLL_THREAD_ATTACH*/: \
				return OnDllMainThreadAttach(); \
			case 3 /*DLL_THREAD_DETACH*/: \
				return OnDllMainThreadDetach(); \
			case 0 /*DLL_PROCESS_DETACH*/: \
				return OnDllMainProcessDetach(); \
			default: \
				return shim::kTRUE; \
		} \
	}

#else

#define RF_EMPTY_DLLMAIN()

#endif

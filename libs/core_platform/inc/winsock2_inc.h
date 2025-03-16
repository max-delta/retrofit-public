#pragma once
#ifndef _WINSOCK2_INC_
#define _WINSOCK2_INC_

#ifdef _WINSOCK2API_
#error "WinSock2 included outside of guard"
#endif

#include "core/compiler.h"

#if defined( RF_PLATFORM_ARM_32 ) || defined( RF_PLATFORM_ARM_64 )
#error "WinSock2.h doesn't compile on ARM as of writing"
#endif

// Need to guard STL before windows destroys it
#include "rftl/cstddef"
#include "rftl/cstring"
#include "rftl/cctype"

// Fix for __uuidof(...) not finding _GUID
#if defined( __clang__ ) && ( __cplusplus >= 202002L )
#include <guiddef.h>
#endif

namespace win32 {
#define NOMINMAX
#define STRICT
#include <WinSock2.h>
#ifdef RF_PLATFORM_MSVC
	// WSAGetIPUserMtu(...) can return uninitialized memory in Windows 10 SDK
	//  version 10.0.19047.0
	#pragma warning( push )
	#pragma warning( disable : 6101 )
	#include <WS2tcpip.h>
	#pragma warning(pop)
#else
	#include <WS2tcpip.h>
#endif
#undef NOMINMAX
#undef STRICT

#undef CALLBACK
#undef near
#undef far
#undef FAR
#undef NEAR
//#undef NULL // Some bugs in the Microsoft STL rely on this
#undef BOOL
#undef TRUE
#undef FALSE
constexpr BOOL FALSE = 0;
constexpr BOOL TRUE = 1;
}

#endif

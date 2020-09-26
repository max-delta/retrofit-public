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

namespace win32 {
#define NOMINMAX
#define STRICT
#include <WinSock2.h>
#include <WS2tcpip.h>
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

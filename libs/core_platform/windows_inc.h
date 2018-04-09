#pragma once
#ifndef _WINDOWS_INC_
#define _WINDOWS_INC_

#ifdef _WINDOWS_
#error "Windows included outside of guard"
#endif

// Need to guard STL before windows destroys it
#include "rftl/cstring"

namespace win32 {
struct IUnknown; // Fix for Windows 8.1 SDK and "/permissive-" flag
#define NOMINMAX
#define STRICT
#include <Windows.h>
#undef NOMINMAX
#undef STRICT

#undef CALLBACK
#undef near
#undef far
#undef FAR
#undef NEAR
#undef NULL
#undef BOOL
#undef TRUE
#undef FALSE
constexpr BOOL FALSE = 0;
constexpr BOOL TRUE = 1;
}

#endif

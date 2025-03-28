#pragma once
#ifndef _WINDOWS_INC_
#define _WINDOWS_INC_

#ifdef _WINDOWS_
#error "Windows included outside of guard"
#endif

#include "core/compiler.h"

#if defined( RF_PLATFORM_ARM_32 ) || defined( RF_PLATFORM_ARM_64 )
#error "Windows.h doesn't compile on ARM as of writing"
#endif

// Need to guard STL before windows destroys it
#include "rftl/cstddef"
#include "rftl/cstdlib"
#include "rftl/cstring"
#include "rftl/cctype"

// Fix for __uuidof(...) not finding _GUID
#if defined( __clang__ ) && ( __cplusplus >= 202002L )
#include <guiddef.h>
#endif

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
//#undef NULL // Some bugs in the Microsoft STL rely on this
#undef BOOL
#undef TRUE
#undef FALSE
constexpr BOOL FALSE = 0;
constexpr BOOL TRUE = 1;
}

#endif

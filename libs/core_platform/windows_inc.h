#pragma once
#ifndef _WINDOWS_INC_
#define _WINDOWS_INC_

#ifdef _WINDOWS_
#error "Windows included outside of guard"
#endif

namespace win32 {
#define NOMINMAX
#define STRICT
#include <Windows.h>
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

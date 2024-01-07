#pragma once
#ifndef _XINPUT_INC_
#define _XINPUT_INC_

#ifdef XUSER_MAX_COUNT
#error "XInput included outside of guard"
#endif

#include "windows_inc.h"

namespace win32 {
#define NOMINMAX
#define STRICT
#include <Xinput.h>
#undef NOMINMAX
#undef STRICT
}

#endif

#pragma once
#ifndef _RPC_INC_
#define _RPC_INC_

#ifdef __RPC_H__
#error "RPC included outside of guard"
#endif

#include "windows_inc.h"

namespace win32 {
#define NOMINMAX
#define STRICT
#include <rpc.h>
#undef NOMINMAX
#undef STRICT
}

#endif

#pragma once
#include "project.h"

#include "rftl/cstdint"


namespace RF::platform::errors {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API uint32_t GetLastSystemErrorOnThread();

///////////////////////////////////////////////////////////////////////////////
}
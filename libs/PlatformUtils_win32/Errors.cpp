#include "stdafx.h"
#include "Errors.h"

#include "core_platform/inc/windows_inc.h"


namespace RF::platform::errors {
///////////////////////////////////////////////////////////////////////////////

uint32_t GetLastSystemErrorOnThread()
{
	return win32::GetLastError();
}

///////////////////////////////////////////////////////////////////////////////
}
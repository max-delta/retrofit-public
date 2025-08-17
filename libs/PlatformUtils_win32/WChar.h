#pragma once
#include "project.h"

#include "rftl/format"
#include "rftl/string"
#include "rftl/string_view"


namespace RF::platform::widechar {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API rftl::u8string ConvertWideChars( rftl::wstring_view string );

///////////////////////////////////////////////////////////////////////////////
}

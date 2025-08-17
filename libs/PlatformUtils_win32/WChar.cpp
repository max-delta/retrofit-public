#include "stdafx.h"
#include "WChar.h"

#include "core_platform/inc/windows_inc.h"

#include "core_unicode/StringConvert.h"


namespace RF::platform::widechar {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API rftl::u8string ConvertWideChars( rftl::wstring_view string )
{
	// Supposedly Windows 2000 switched from UCS-2 to UTF-16
	static_assert( sizeof( rftl::wstring_view::value_type ) == sizeof( rftl::u16string_view::value_type ) );
	rftl::u16string_view const assumeUTF16( reinterpret_cast<char16_t const*>( string.data() ), string.size() );
	return unicode::ConvertToUtf8( assumeUTF16 );
}

///////////////////////////////////////////////////////////////////////////////
}

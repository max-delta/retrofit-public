#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/string_view"
#include "rftl/string"
#include "rftl/deque"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API CsvReader
{
	RF_NO_INSTANCE( CsvReader );

public:
	static constexpr char kDefaultSeparator = ',';

public:
	static rftl::deque<rftl::deque<rftl::basic_string<char>>> TokenizeToDeques( rftl::string_view const& buffer );
	static rftl::deque<rftl::deque<rftl::basic_string<char16_t>>> TokenizeToDeques( rftl::u16string_view const& buffer );
	static rftl::deque<rftl::deque<rftl::basic_string<char32_t>>> TokenizeToDeques( rftl::u32string_view const& buffer );

	// Optional override for alternative separators instead of commas
	static rftl::deque<rftl::deque<rftl::basic_string<char>>> TokenizeToDeques( rftl::string_view const& buffer, char separator );
	static rftl::deque<rftl::deque<rftl::basic_string<char16_t>>> TokenizeToDeques( rftl::u16string_view const& buffer, char16_t separator );
	static rftl::deque<rftl::deque<rftl::basic_string<char32_t>>> TokenizeToDeques( rftl::u32string_view const& buffer, char32_t separator );
};

///////////////////////////////////////////////////////////////////////////////
}

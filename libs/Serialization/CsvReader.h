#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/string_view"
#include "rftl/string"
#include "rftl/deque"


namespace RF { namespace serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API CsvReader
{
	RF_NO_INSTANCE( CsvReader );

public:
	static rftl::deque<rftl::deque<rftl::basic_string<char>>> TokenizeToDeques( rftl::string_view const& buffer );
	static rftl::deque<rftl::deque<rftl::basic_string<char16_t>>> TokenizeToDeques( rftl::u16string_view const& buffer );
	static rftl::deque<rftl::deque<rftl::basic_string<char32_t>>> TokenizeToDeques( rftl::u32string_view const& buffer );
};

///////////////////////////////////////////////////////////////////////////////
}}

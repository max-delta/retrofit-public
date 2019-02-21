#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/string"
#include "rftl/deque"


namespace RF { namespace serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API CsvReader
{
	RF_NO_INSTANCE( CsvReader );

public:
	static rftl::deque<rftl::deque<rftl::basic_string<char>>> TokenizeToDeques( char const* buffer, size_t bufferLen );
	static rftl::deque<rftl::deque<rftl::basic_string<char16_t>>> TokenizeToDeques( char16_t const* buffer, size_t bufferLen );
	static rftl::deque<rftl::deque<rftl::basic_string<char32_t>>> TokenizeToDeques( char32_t const* buffer, size_t bufferLen );
};

///////////////////////////////////////////////////////////////////////////////
}}

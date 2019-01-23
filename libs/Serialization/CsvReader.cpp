#include "stdafx.h"
#include "CsvReader.h"


namespace RF { namespace serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT>
rftl::deque<rftl::deque<rftl::basic_string<CharT>>> TokenizeToDeques( CharT const* buffer, size_t bufferLen )
{
	rftl::deque<rftl::deque<rftl::basic_string<CharT>>> retVal;
	retVal.emplace_back();
	rftl::basic_string<CharT> curField;
	for( size_t i = 0; i < bufferLen; i++ )
	{
		CharT const ch = buffer[i];

		if( ch == '\r' )
		{
			// Ignore \r\n's \r
			continue;
		}
		else if( ch == ',' )
		{
			retVal.back().emplace_back( std::move( curField ) );
			curField.clear();
		}
		else if( ch == '\n' )
		{
			if( curField.empty() && retVal.back().empty() )
			{
				// Ignore empty lines
			}
			else
			{
				retVal.back().emplace_back( std::move( curField ) );
				curField.clear();
				retVal.emplace_back();
			}
		}
		else
		{
			curField.push_back( ch );
		}
	}

	if( retVal.back().empty() )
	{
		// Ignore trailing empty lines
		// NOTE: Also implicitly covers empty file case
		retVal.pop_back();
	}

	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

rftl::deque<rftl::deque<rftl::basic_string<char>>> CsvReader::TokenizeToDeques( char const* buffer, size_t bufferLen )
{
	return details::TokenizeToDeques( buffer, bufferLen );
}



rftl::deque<rftl::deque<rftl::basic_string<char16_t>>> CsvReader::TokenizeToDeques( char16_t const* buffer, size_t bufferLen )
{
	return details::TokenizeToDeques( buffer, bufferLen );
}



rftl::deque<rftl::deque<rftl::basic_string<char32_t>>> CsvReader::TokenizeToDeques( char32_t const* buffer, size_t bufferLen )
{
	return details::TokenizeToDeques( buffer, bufferLen );
}

///////////////////////////////////////////////////////////////////////////////
}}

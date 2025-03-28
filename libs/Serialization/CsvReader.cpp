#include "stdafx.h"
#include "CsvReader.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT>
rftl::deque<rftl::deque<rftl::basic_string<CharT>>> TokenizeToDeques( rftl::basic_string_view<CharT> const& buffer, CharT separator )
{
	rftl::deque<rftl::deque<rftl::basic_string<CharT>>> retVal;
	retVal.emplace_back();
	rftl::basic_string<CharT> curField;
	for( CharT const& ch : buffer )
	{
		if( ch == '\r' )
		{
			// Ignore \r\n's \r
			continue;
		}
		else if( ch == separator )
		{
			retVal.back().emplace_back( rftl::move( curField ) );
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
				retVal.back().emplace_back( rftl::move( curField ) );
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

rftl::deque<rftl::deque<rftl::basic_string<char>>> CsvReader::TokenizeToDeques( rftl::string_view const& buffer )
{
	return TokenizeToDeques( buffer, kDefaultSeparator );
}



rftl::deque<rftl::deque<rftl::basic_string<char16_t>>> CsvReader::TokenizeToDeques( rftl::u16string_view const& buffer )
{
	return TokenizeToDeques( buffer, kDefaultSeparator );
}



rftl::deque<rftl::deque<rftl::basic_string<char32_t>>> CsvReader::TokenizeToDeques( rftl::u32string_view const& buffer )
{
	return TokenizeToDeques( buffer, kDefaultSeparator );
}



rftl::deque<rftl::deque<rftl::basic_string<char>>> CsvReader::TokenizeToDeques( rftl::string_view const& buffer, char separator )
{
	return details::TokenizeToDeques( buffer, separator );
}



rftl::deque<rftl::deque<rftl::basic_string<char16_t>>> CsvReader::TokenizeToDeques( rftl::u16string_view const& buffer, char16_t separator )
{
	return details::TokenizeToDeques( buffer, separator );
}



rftl::deque<rftl::deque<rftl::basic_string<char32_t>>> CsvReader::TokenizeToDeques( rftl::u32string_view const& buffer, char32_t separator )
{
	return details::TokenizeToDeques( buffer, separator );
}

///////////////////////////////////////////////////////////////////////////////
}

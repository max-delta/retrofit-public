#include "stdafx.h"
#include "StringConvert.h"
#include "InsertConvert.h"

#include "rftl/iterator"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToASCII( rftl::string_view source )
{
	rftl::string retVal;
	retVal.reserve( source.size() );
	ConvertToASCII( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::string ConvertToASCII( rftl::u8string_view source )
{
	rftl::string retVal;
	retVal.reserve( source.size() );
	ConvertToASCII( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::string ConvertToASCII( rftl::u16string_view source )
{
	rftl::string retVal;
	retVal.reserve( source.size() );
	ConvertToASCII( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::string ConvertToASCII( rftl::u32string_view source )
{
	rftl::string retVal;
	retVal.reserve( source.size() );
	ConvertToASCII( source, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::u8string ConvertToUtf8( rftl::string_view source )
{
	rftl::u8string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf8( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u8string ConvertToUtf8( rftl::u8string_view source )
{
	rftl::u8string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf8( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u8string ConvertToUtf8( rftl::u16string_view source )
{
	rftl::u8string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf8( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u8string ConvertToUtf8( rftl::u32string_view source )
{
	rftl::u8string retVal;
	retVal.reserve( source.size() * 4 );
	ConvertToUtf8( source, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::u16string ConvertToUtf16( rftl::string_view source )
{
	rftl::u16string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf16( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u16string ConvertToUtf16( rftl::u8string_view source )
{
	rftl::u16string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf16( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u16string ConvertToUtf16( rftl::u16string_view source )
{
	rftl::u16string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf16( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u16string ConvertToUtf16( rftl::u32string_view source )
{
	rftl::u16string retVal;
	retVal.reserve( source.size() * 2 );
	ConvertToUtf16( source, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::u32string ConvertToUtf32( rftl::string_view source )
{
	rftl::u32string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf32( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u32string ConvertToUtf32( rftl::u8string_view source )
{
	rftl::u32string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf32( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u32string ConvertToUtf32( rftl::u16string_view source )
{
	rftl::u32string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf32( source, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u32string ConvertToUtf32( rftl::u32string_view source )
{
	rftl::u32string retVal;
	retVal.reserve( source.size() );
	ConvertToUtf32( source, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

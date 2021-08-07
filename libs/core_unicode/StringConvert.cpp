#include "stdafx.h"
#include "StringConvert.h"
#include "InsertConvert.h"

#include "rftl/iterator"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToASCII( char const* source, size_t numBytes )
{
	rftl::string retVal;
	retVal.reserve( numBytes );
	ConvertToASCII( source, numBytes, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::string ConvertToASCII( char16_t const* source, size_t numPairs )
{
	rftl::string retVal;
	retVal.reserve( numPairs );
	ConvertToASCII( source, numPairs, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::string ConvertToASCII( char32_t const* source, size_t numCodePoints )
{
	rftl::string retVal;
	retVal.reserve( numCodePoints );
	ConvertToASCII( source, numCodePoints, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToUtf8( char const* source, size_t numBytes )
{
	rftl::string retVal;
	retVal.reserve( numBytes );
	ConvertToUtf8( source, numBytes, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::string ConvertToUtf8( char16_t const* source, size_t numPairs )
{
	rftl::string retVal;
	retVal.reserve( numPairs );
	ConvertToUtf8( source, numPairs, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::string ConvertToUtf8( char32_t const* source, size_t numCodePoints )
{
	rftl::string retVal;
	retVal.reserve( numCodePoints * 4 );
	ConvertToUtf8( source, numCodePoints, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::u16string ConvertToUtf16( char const* source, size_t numBytes )
{
	rftl::u16string retVal;
	retVal.reserve( numBytes );
	ConvertToUtf16( source, numBytes, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u16string ConvertToUtf16( char16_t const* source, size_t numPairs )
{
	rftl::u16string retVal;
	retVal.reserve( numPairs );
	ConvertToUtf16( source, numPairs, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u16string ConvertToUtf16( char32_t const* source, size_t numCodePoints )
{
	rftl::u16string retVal;
	retVal.reserve( numCodePoints * 2 );
	ConvertToUtf16( source, numCodePoints, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::u32string ConvertToUtf32( char const* source, size_t numBytes )
{
	rftl::u32string retVal;
	retVal.reserve( numBytes );
	ConvertToUtf32( source, numBytes, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u32string ConvertToUtf32( char16_t const* source, size_t numPairs )
{
	rftl::u32string retVal;
	retVal.reserve( numPairs );
	ConvertToUtf32( source, numPairs, rftl::back_inserter( retVal ) );
	return retVal;
}



rftl::u32string ConvertToUtf32( char32_t const* source, size_t numCodePoints )
{
	rftl::u32string retVal;
	retVal.reserve( numCodePoints );
	ConvertToUtf32( source, numCodePoints, rftl::back_inserter( retVal ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToASCII( rftl::string const& source )
{
	return ConvertToASCII( source.data(), source.size() );
}



rftl::string ConvertToASCII( rftl::u16string const& source )
{
	return ConvertToASCII( source.data(), source.size() );
}



rftl::string ConvertToASCII( rftl::u32string const& source )
{
	return ConvertToASCII( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToUtf8( rftl::string const& source )
{
	return ConvertToUtf8( source.data(), source.size() );
}



rftl::string ConvertToUtf8( rftl::u16string const& source )
{
	return ConvertToUtf8( source.data(), source.size() );
}



rftl::string ConvertToUtf8( rftl::u32string const& source )
{
	return ConvertToUtf8( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////

rftl::u16string ConvertToUtf16( rftl::string const& source )
{
	return ConvertToUtf16( source.data(), source.size() );
}



rftl::u16string ConvertToUtf16( rftl::u16string const& source )
{
	return ConvertToUtf16( source.data(), source.size() );
}



rftl::u16string ConvertToUtf16( rftl::u32string const& source )
{
	return ConvertToUtf16( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////

rftl::u32string ConvertToUtf32( rftl::string const& source )
{
	return ConvertToUtf32( source.data(), source.size() );
}



rftl::u32string ConvertToUtf32( rftl::u16string const& source )
{
	return ConvertToUtf32( source.data(), source.size() );
}



rftl::u32string ConvertToUtf32( rftl::u32string const& source )
{
	return ConvertToUtf32( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////
}

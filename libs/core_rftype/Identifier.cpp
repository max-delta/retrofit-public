#include "stdafx.h"
#include "Identifier.h"

#include "core/macros.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

bool IsValidIdentifier( rftl::string_view name )
{
	if( name.empty() )
	{
		return false;
	}

	size_t const index = GetInvalidIdentifierCharacterIndex( name );
	return index >= name.size();
}



size_t GetInvalidIdentifierCharacterIndex( rftl::string_view name )
{
	if( name.empty() )
	{
		return 0;
	}

	size_t testIndex = 0;
	for( testIndex = 0; testIndex < name.size(); testIndex++ )
	{
		char const& ch = name[testIndex];

		if( ch >= 'a' && ch <= 'z' )
		{
			continue;
		}

		if( ch >= 'A' && ch <= 'Z' )
		{
			continue;
		}

		if( ch >= '0' && ch <= '9' )
		{
			if( testIndex == 0 )
			{
				break;
			}
			continue;
		}

		if( ch == '_' )
		{
			continue;
		}

		break;
	}
	return testIndex;
}



void SanitizeIdentifier( rftl::string_view in, char* out )
{
	for( size_t offset = 0; offset < in.size(); offset++ )
	{
		char const& inC = in[offset];
		char& outC = out[offset];
		if( inC == ':' )
		{
			outC = '_';
		}
		else
		{
			outC = inC;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}

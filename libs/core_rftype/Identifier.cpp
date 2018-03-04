#include "stdafx.h"
#include "Identifier.h"

#include "core/macros.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

bool IsValidIdentifier( char const* name )
{
	RF_ASSERT( name != nullptr );
	size_t const index = GetInvalidIdentifierCharacterIndex( name );
	char const ch = name[index];
	return ch == '\0' && index != 0;
}



size_t GetInvalidIdentifierCharacterIndex( char const* name )
{
	RF_ASSERT( name != nullptr );
	char ch = '\255';
	size_t previousIndex = 0;
	while( ch != '\0' )
	{
		previousIndex++;
		size_t const currentIndex = previousIndex - 1;
		ch = name[currentIndex];

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
			if( currentIndex == 0 )
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
	return previousIndex - 1;
}

///////////////////////////////////////////////////////////////////////////////
}}

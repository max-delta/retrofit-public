#pragma once


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

bool IsValidIdentifier( char const* name );
size_t GetInvalidIdentifierCharacterIndex( char const* name );

void SanitizeIdentifier( char const* in, char* out );

///////////////////////////////////////////////////////////////////////////////
}

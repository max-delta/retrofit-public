#pragma once

#include "rftl/string_view"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

bool IsValidIdentifier( rftl::string_view name );
size_t GetInvalidIdentifierCharacterIndex( rftl::string_view name );

void SanitizeIdentifier( rftl::string_view in, char* out );

///////////////////////////////////////////////////////////////////////////////
}

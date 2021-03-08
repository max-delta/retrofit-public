#include "stdafx.h"
#include "TextInputComponent.h"

#include "rftl/limits"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

void TextInputComponent::GetTextStream( rftl::u16string& text ) const
{
	return GetTextStream( text, rftl::numeric_limits<size_t>::max() );
}

///////////////////////////////////////////////////////////////////////////////
}

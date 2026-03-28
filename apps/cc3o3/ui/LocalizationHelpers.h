#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/extension/unique_char_set.h"
#include "rftl/string"
#include "rftl/string_view"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection();
rftl::unique_char_set GetBreakableChars();
rftl::unique_char_set GetConsumableChars();

rftl::string LocalizeKey( char const* key );
rftl::string LocalizeKey( rftl::string_view const& key );
rftl::string LocalizeKey( rftl::string&& key );

///////////////////////////////////////////////////////////////////////////////
}

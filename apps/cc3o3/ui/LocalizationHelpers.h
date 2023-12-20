#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"
#include "rftl/string_view"
#include "rftl/unordered_set"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection();
rftl::unordered_set<char> GetBreakableChars();

rftl::string LocalizeKey( rftl::string_view const& key );
rftl::string LocalizeKey( rftl::string&& key );

///////////////////////////////////////////////////////////////////////////////
}

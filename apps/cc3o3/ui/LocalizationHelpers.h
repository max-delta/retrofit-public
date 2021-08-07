#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"
#include "rftl/unordered_set"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection();
rftl::unordered_set<char> GetBreakableChars();

rftl::string LocalizeKey( rftl::string const& key );
rftl::string LocalizeKey( rftl::string&& key );

///////////////////////////////////////////////////////////////////////////////
}

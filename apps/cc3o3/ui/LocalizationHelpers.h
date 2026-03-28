#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"
#include "rftl/string_view"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection();
loc::LineBreakRules GetLineBreakRules();

rftl::string LocalizeKey( char const* key );
rftl::string LocalizeKey( rftl::string_view const& key );
rftl::string LocalizeKey( rftl::string&& key );

///////////////////////////////////////////////////////////////////////////////
}

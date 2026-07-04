#pragma once
#include "project.h"

#include "core_localization/LocFwd.h"

#include "rftl/string"
#include "rftl/string_view"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection();
loc::LineBreakRules GetLineBreakRules();

rftl::string LocalizeKey( rftl::string_view key );
rftl::string LocalizeDynamicTarget( rftl::u32string_view target );

///////////////////////////////////////////////////////////////////////////////
}

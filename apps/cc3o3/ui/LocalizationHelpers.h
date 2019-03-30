#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"


namespace RF { namespace cc { namespace ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection();

rftl::string LocalizeKey( rftl::string const& key );
rftl::string LocalizeKey( rftl::string&& key );

///////////////////////////////////////////////////////////////////////////////
}}}

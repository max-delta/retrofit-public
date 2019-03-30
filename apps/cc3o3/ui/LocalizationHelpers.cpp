#include "stdafx.h"
#include "LocalizationHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "Localization/LocEngine.h"
#include "Localization/PageMapper.h"
#include "Localization/LocQuery.h"
#include "Localization/LocResult.h"

#include "core/ptr/weak_ptr.h"


namespace RF { namespace cc { namespace ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection()
{
	return app::gLocEngine->GetTextDirection();
}



rftl::string LocalizeKey( rftl::string const& key )
{
	return app::gPageMapper->MapTo8Bit( app::gLocEngine->Query( loc::LocQuery( loc::LocKey( key ) ) ).GetCodePoints() );
}



rftl::string LocalizeKey( rftl::string&& key )
{
	return app::gPageMapper->MapTo8Bit( app::gLocEngine->Query( loc::LocQuery( loc::LocKey( rftl::move( key ) ) ) ).GetCodePoints() );
}

///////////////////////////////////////////////////////////////////////////////
}}}

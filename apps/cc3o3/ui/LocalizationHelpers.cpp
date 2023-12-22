#include "stdafx.h"
#include "LocalizationHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "Localization/LocEngine.h"
#include "Localization/PageMapper.h"
#include "Localization/LocQuery.h"
#include "Localization/LocResult.h"

#include "core/ptr/weak_ptr.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

loc::TextDirection GetTextDirection()
{
	return app::gLocEngine->GetTextDirection();
}



rftl::unordered_set<char> GetBreakableChars()
{
	// TODO: Get from LocEngine
	return { ' ', '-' };
}



rftl::string LocalizeKey( char const* key )
{
	return LocalizeKey( rftl::string_view( key ) );
}



rftl::string LocalizeKey( rftl::string_view const& key )
{
	return app::gPageMapper->MapTo8Bit( app::gLocEngine->Query( loc::LocQuery( loc::LocKey( key ) ) ).GetCodePoints() );
}



rftl::string LocalizeKey( rftl::string&& key )
{
	return app::gPageMapper->MapTo8Bit( app::gLocEngine->Query( loc::LocQuery( loc::LocKey( rftl::move( key ) ) ) ).GetCodePoints() );
}

///////////////////////////////////////////////////////////////////////////////
}

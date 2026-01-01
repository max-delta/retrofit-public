#pragma once
#include "project.h"

#include "AppCommon_GraphicalClient/StartupConfig.h"


namespace RF::cc::init {
///////////////////////////////////////////////////////////////////////////////

struct StartupConfig : public app::StartupConfig
{
	static StartupConfig FetchFromBinary();

	// No additional config at this time
};

///////////////////////////////////////////////////////////////////////////////
}

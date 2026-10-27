#pragma once
#include "project.h"

#include "cc3o3/scene/SceneFwd.h"

#include "PlatformFilesystem/VFSPath.h"


namespace RF::cc::scene {
///////////////////////////////////////////////////////////////////////////////

class SceneLoader
{
	RF_NO_INSTANCE( SceneLoader );
public:
	static scene::Scene LoadFromDesc( file::VFSPath const& descPath );
};

///////////////////////////////////////////////////////////////////////////////
}

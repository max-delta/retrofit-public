#include "stdafx.h"
#include "SceneLoader.h"

#include "cc3o3/resource/ResourceLoad.h"

#include "GameScene/Scene.h"


namespace RF::cc::scene {
///////////////////////////////////////////////////////////////////////////////

scene::Scene SceneLoader::LoadFromDesc( file::VFSPath const& descPath )
{
	using namespace scene;

	UniquePtr<SceneDesc const> descPtr = resource::LoadFromFile<SceneDesc const>( descPath );
	RFLOG_TEST_AND_FATAL( descPtr != nullptr, descPath, RFCAT_CC3O3, "Failed to load scene desc" );
	SceneDesc const& desc = *descPtr;

	return Scene::CreateFromDesc( desc );
}

///////////////////////////////////////////////////////////////////////////////
}

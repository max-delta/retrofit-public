#include "stdafx.h"
#include "Scene.h"

#include "cc3o3/scene/SceneDesc.h"
#include "cc3o3/resource/ResourceLoad.h"


namespace RF::cc::scene {
///////////////////////////////////////////////////////////////////////////////

Scene Scene::LoadFromDesc( file::VFSPath const& descPath )
{
	Scene retVal = {};

	UniquePtr<SceneDesc const> descPtr = resource::LoadFromFile<SceneDesc const>( descPath );
	RFLOG_TEST_AND_FATAL( descPtr != nullptr, descPath, RFCAT_CC3O3, "Failed to load scene desc" );
	SceneDesc const& desc = *descPtr;

	// Layers
	retVal.mLayers.reserve( desc.mLayers.size() );
	for( LayerDesc const& entityDesc : desc.mLayers )
	{
		Layer layer = {};
		layer.mTileset = file::VFSPath::CreatePathFromString( entityDesc.mTileset );
		layer.mTileLayer = file::VFSPath::CreatePathFromString( entityDesc.mTileLayer );
		layer.mMode = entityDesc.mMode;
		layer.mDepth = entityDesc.mDepth;
		layer.mParallax = entityDesc.mParallax;
		retVal.mLayers.emplace_back( rftl::move( layer ) );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

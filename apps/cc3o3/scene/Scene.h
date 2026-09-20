#pragma once
#include "cc3o3/scene/SceneFwd.h"

#include "PlatformFilesystem/VFSPath.h"

#include "rftl/vector"


namespace RF::cc::scene {
///////////////////////////////////////////////////////////////////////////////

class Layer
{
	//
	// Public data
public:
	file::VFSPath mTileset;
	file::VFSPath mTileLayer;
	rftl::string mMode;
	int8_t mDepth = 0;
	uint8_t mParallax = 0;
};

///////////////////////////////////////////////////////////////////////////////

class Scene
{
	//
	// Types and constants
public:
	using Layers = rftl::vector<Layer>;


	//
	// Public methods
public:
	Scene() = default;

	static Scene LoadFromDesc( file::VFSPath const& descPath );


	//
	// Public data
public:
	Layers mLayers;
};

///////////////////////////////////////////////////////////////////////////////
}

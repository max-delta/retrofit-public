#pragma once
#include "cc3o3/scene/SceneFwd.h"

#include "PlatformFilesystem/VFSPath.h"
#include "PPU/TileLayer.h"

#include "rftl/vector"


namespace RF::cc::scene {
///////////////////////////////////////////////////////////////////////////////

class LayerResources
{
	RF_NO_COPY( LayerResources );
	RF_NO_ASSIGN_VIA_MOVE( LayerResources );

	//
	// Friends
private:
	friend class Layer;


	//
	// Public methods
public:
	LayerResources() = default;
	LayerResources( LayerResources&& rhs );
	~LayerResources();

	bool Unload();


	//
	// Public data
public:
	gfx::ppu::TileLayer mTileLayer = {};

	//
	// Private data
private:
	gfx::ManagedTilesetID mLoadedTileset = gfx::kInvalidManagedTilesetID;
	rftl::string mTilesetResourceName = {};
};

///////////////////////////////////////////////////////////////////////////////

class Layer
{
	RF_NO_COPY( Layer );
	RF_NO_ASSIGN_VIA_MOVE( Layer );
	RF_DEFAULT_CONSTRUCT_VIA_MOVE( Layer );

	//
	// Public methods
public:
	Layer() = default;
	~Layer() = default;

	// Loading is deferred until actually requested, and can be ejected upon
	//  request as well
	bool LoadResources( rftl::string&& resourceName );


	//
	// Public data
public:
	file::VFSPath mTilesetPath;
	file::VFSPath mTileLayerPath;
	rftl::string mMode;
	int8_t mDepth = 0;
	uint8_t mParallax = 0;

	LayerResources mResources = {};
};

///////////////////////////////////////////////////////////////////////////////

class Scene
{
	RF_NO_COPY( Scene );
	RF_DEFAULT_MOVE( Scene );

	//
	// Types and constants
public:
	using Layers = rftl::vector<Layer>;


	//
	// Public methods
public:
	Scene() = default;

	// Loads the metadata, not the resources
	static Scene LoadFromDesc( file::VFSPath const& descPath );

	// Load / unload the actual resources
	// NOTE: Returns true only on total success, a partial failure returns
	//  false, though attempts to perform the operation for all layers
	//  regardless
	bool LoadResources( rftl::string_view resourcePrefix );
	bool UnloadResources();


	//
	// Public data
public:
	Layers mLayers;
};

///////////////////////////////////////////////////////////////////////////////
}

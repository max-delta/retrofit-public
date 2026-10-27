#pragma once
#include "GameScene/SceneFwd.h"

#include "PlatformFilesystem/VFSPath.h"
#include "PPU/TileLayer.h"

#include "rftl/vector"


namespace RF::scene {
///////////////////////////////////////////////////////////////////////////////

// Resources for a layer, the saturated form of a layer that is actually usable
class GAMESCENE_API LayerResources
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
	WeakPtr<gfx::ppu::PPUController> mPpu;
	gfx::ManagedTilesetID mLoadedTileset = gfx::kInvalidManagedTilesetID;
	rftl::string mTilesetResourceName = {};
};

///////////////////////////////////////////////////////////////////////////////

// Layer within a scene, used for separating tile layers by purpose or depth
class GAMESCENE_API Layer
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
	bool LoadResources(
		WeakPtr<gfx::ppu::PPUController> ppu,
		file::VFS& vfs,
		rftl::string&& resourceName );


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

// A collection of tile layers, with no further assumptions beyond that,
//  relying on consuming code to understand the intent of the layers and mores
class GAMESCENE_API Scene
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
	static Scene CreateFromDesc( SceneDesc const& desc );

	// Load / unload the actual resources
	// NOTE: Returns true only on total success, a partial failure returns
	//  false, though attempts to perform the operation for all layers
	//  regardless
	bool LoadResources(
		WeakPtr<gfx::ppu::PPUController> ppu,
		file::VFS& vfs,
		rftl::string_view resourcePrefix );
	bool UnloadResources();

	// Runs a filter on the mode, meant to be used for a common case of looking
	//  for a specific layer when there should only be one of them
	Layer const* FindFirstOf( rftl::string_view mode ) const;
	Layer* FindFirstOfMutable( rftl::string_view mode );


	//
	// Public data
public:
	Layers mLayers;
};

///////////////////////////////////////////////////////////////////////////////
}

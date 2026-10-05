#include "stdafx.h"
#include "Scene.h"

#include "cc3o3/scene/SceneDesc.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/TileLayerCSVLoader.h"


namespace RF::cc::scene {
///////////////////////////////////////////////////////////////////////////////

LayerResources::LayerResources( LayerResources&& rhs )
	: RF_MOVE_CONSTRUCT( mTileLayer )
	, RF_MOVE_CONSTRUCT( mLoadedTileset )
	, RF_MOVE_CONSTRUCT( mTilesetResourceName )
{
	RF_MOVE_CLEAR( mTileLayer );
	RF_MOVE_CLEAR( mLoadedTileset );
	RF_MOVE_CLEAR( mTilesetResourceName );
}



LayerResources::~LayerResources()
{
	if( mLoadedTileset != gfx::kInvalidManagedTilesetID )
	{
		Unload();
	}

	RF_ASSERT( mTileLayer.mTilesetReference == mLoadedTileset );
	RF_ASSERT( mLoadedTileset == gfx::kInvalidManagedTilesetID );
	RF_ASSERT( mTilesetResourceName.empty() );
}



bool LayerResources::Unload()
{
	RF_ASSERT_MSG( mTilesetResourceName.empty() == false, "Already unnamed" );
	RF_ASSERT_MSG( mLoadedTileset != gfx::kInvalidManagedTilesetID, "Already unloaded" );
	RF_ASSERT_MSG( mTileLayer.mTilesetReference == mLoadedTileset, "Layer reference is divergent" );

	gfx::ppu::PPUController& ppu = *app::gGraphics;
	gfx::TilesetManager& tsetMan = *ppu.DebugGetTilesetManager();

	bool const destroySuccess = tsetMan.DestroyResource( mTilesetResourceName );
	RF_ASSERT( destroySuccess );

	mTileLayer = {};
	mLoadedTileset = gfx::kInvalidManagedTilesetID;
	mTilesetResourceName.clear();

	return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Layer::LoadResources( rftl::string&& resourceName )
{
	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT_MSG( mResources.mTilesetResourceName.empty(), "Already named" );
	RF_ASSERT_MSG( mResources.mLoadedTileset == gfx::kInvalidManagedTilesetID, "Already loaded" );
	RF_ASSERT_MSG( mResources.mTileLayer.mTilesetReference == mResources.mLoadedTileset, "Layer reference is divergent" );

	gfx::ppu::PPUController& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	file::VFS& vfs = *app::gVfs;

	mResources.mTilesetResourceName = rftl::move( resourceName );

	bool const tilesetLoaded =
		ppu.ForceImmediateLoadRequest(
			gfx::ppu::PPUController::AssetType::Tileset, mResources.mTilesetResourceName, mTilesetPath );
	RF_ASSERT( tilesetLoaded );
	mResources.mLoadedTileset = tsetMan.GetManagedResourceIDFromResourceName( mResources.mTilesetResourceName );
	RF_ASSERT( mResources.mLoadedTileset != gfx::kInvalidManagedTilesetID );

	gfx::ppu::TileLayer& tileLayer = mResources.mTileLayer;
	tileLayer = {};
	tileLayer.mTilesetReference = mResources.mLoadedTileset;
	tileLayer.mTileZoomFactor = gfx::ppu::TileLayer::kTileZoomFactor_Normal;
	tileLayer.mXCoord = 0;
	tileLayer.mYCoord = 0;
	tileLayer.mZLayer = mDepth;
	tileLayer.mHorizontalWrapping = false;
	tileLayer.mVerticalWrapping = false;
	tileLayer.mLooping = true;

	bool const tilesLoaded =
		gfx::ppu::TileLayerCSVLoader::LoadTiles(
			tileLayer, vfs, mTileLayerPath );
	RF_ASSERT( tilesLoaded );

	return true;
}

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
		layer.mTilesetPath = file::VFSPath::CreatePathFromString( entityDesc.mTileset );
		layer.mTileLayerPath = file::VFSPath::CreatePathFromString( entityDesc.mTileLayer );
		layer.mMode = entityDesc.mMode;
		layer.mDepth = entityDesc.mDepth;
		layer.mParallax = entityDesc.mParallax;
		retVal.mLayers.emplace_back( rftl::move( layer ) );
	}

	return retVal;
}



bool Scene::LoadResources( rftl::string_view resourcePrefix )
{
	RF_ASSERT( resourcePrefix.empty() == false );

	bool retVal = true;
	size_t layerIndex = 0;
	for( Layer& layer : mLayers )
	{
		rftl::string resourceName = rftl::format( "{}_layer{}", resourcePrefix, layerIndex );
		layerIndex++;
		bool const success = layer.LoadResources( rftl::move( resourceName ) );
		retVal = retVal && success;
	}
	return retVal;
}



bool Scene::UnloadResources()
{
	bool retVal = true;
	for( Layer& layer : mLayers )
	{
		bool const success = layer.mResources.Unload();
		retVal = retVal && success;
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

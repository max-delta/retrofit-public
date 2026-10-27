#include "stdafx.h"
#include "Scene.h"

#include "GameScene/SceneDesc.h"

#include "PPU/PPUController.h"
#include "PPU/TileLayerCSVLoader.h"


namespace RF::scene {
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
	RF_ASSERT_MSG( mPpu != nullptr, "Already unbound" );
	RF_ASSERT_MSG( mTilesetResourceName.empty() == false, "Already unnamed" );
	RF_ASSERT_MSG( mLoadedTileset != gfx::kInvalidManagedTilesetID, "Already unloaded" );
	RF_ASSERT_MSG( mTileLayer.mTilesetReference == mLoadedTileset, "Layer reference is divergent" );

	gfx::ppu::PPUController& ppu = *mPpu;

	bool const destroySuccess = ppu.QueueDeferredDestroyRequest( gfx::ppu::AssetType::Tileset, mTilesetResourceName );
	RF_ASSERT( destroySuccess );

	mTileLayer = {};
	mPpu = nullptr;
	mLoadedTileset = gfx::kInvalidManagedTilesetID;
	mTilesetResourceName.clear();

	return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Layer::LoadResources( WeakPtr<gfx::ppu::PPUController> ppu, file::VFS& vfs, rftl::string&& resourceName )
{
	RF_ASSERT( ppu != nullptr );
	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT_MSG( mResources.mPpu == nullptr, "Already bound to ppu" );
	RF_ASSERT_MSG( mResources.mTilesetResourceName.empty(), "Already named" );
	RF_ASSERT_MSG( mResources.mLoadedTileset == gfx::kInvalidManagedTilesetID, "Already loaded" );
	RF_ASSERT_MSG( mResources.mTileLayer.mTilesetReference == mResources.mLoadedTileset, "Layer reference is divergent" );

	mResources.mPpu = ppu;
	gfx::ppu::PPUController& ppuRef = *mResources.mPpu;

	mResources.mTilesetResourceName = rftl::move( resourceName );

	mResources.mLoadedTileset = ppuRef.QueueDeferredLoadRequest<gfx::ppu::AssetType::Tileset>( mResources.mTilesetResourceName, mTilesetPath );
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

Scene Scene::CreateFromDesc( SceneDesc const& desc )
{
	Scene retVal = {};

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



bool Scene::LoadResources(
	WeakPtr<gfx::ppu::PPUController> ppu,
	file::VFS& vfs,
	rftl::string_view resourcePrefix )
{
	RF_ASSERT( resourcePrefix.empty() == false );

	bool retVal = true;
	size_t layerIndex = 0;
	for( Layer& layer : mLayers )
	{
		rftl::string resourceName = rftl::format( "{}_layer{}", resourcePrefix, layerIndex );
		layerIndex++;
		bool const success = layer.LoadResources( ppu, vfs, rftl::move( resourceName ) );
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



Layer const* Scene::FindFirstOf( rftl::string_view mode ) const
{
	for( Layer const& layer : mLayers )
	{
		if( layer.mMode == mode )
		{
			return &layer;
		}
	}
	return nullptr;
}



Layer* Scene::FindFirstOfMutable( rftl::string_view mode )
{
	return const_cast<Layer*>( FindFirstOf( mode ) );
}

///////////////////////////////////////////////////////////////////////////////
}

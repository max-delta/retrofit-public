#include "stdafx.h"
#include "Overworld.h"

#include "cc3o3/overworld/OverworldDesc.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/Bitmap.h"
#include "GameSprite/BitmapReader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"


namespace RF::cc::overworld {
///////////////////////////////////////////////////////////////////////////////

Overworld Overworld::LoadFromDesc( file::VFSPath const& descPath )
{
	Overworld retVal = {};

	UniquePtr<OverworldDesc const> descPtr = resource::LoadFromFile<OverworldDesc const>( descPath );
	RFLOG_TEST_AND_FATAL( descPtr != nullptr, descPath, RFCAT_CC3O3, "Failed to load overworld desc" );
	OverworldDesc const& desc = *descPtr;

	// Load collision map
	{
		// Load as color
		sprite::Bitmap colorBitmap( ExplicitDefaultConstruct{} );
		{
			file::VFSPath const collisionPath = file::VFSPath::CreatePathFromString( desc.mCollisionMapPath );
			file::FileHandlePtr const handle = app::gVfs->GetFileForRead( collisionPath );
			RF_ASSERT( handle != nullptr );
			file::FileBuffer const buffer( *handle, false );
			colorBitmap = sprite::BitmapReader::ReadRGBABitmap( buffer.GetBytes() );
		}

		retVal.mCollisionMap = math::Bitmap( colorBitmap.GetWidth(), colorBitmap.GetHeight() );
		for( size_t y = 0; y < colorBitmap.GetHeight(); y++ )
		{
			for( size_t x = 0; x < colorBitmap.GetWidth(); x++ )
			{
				math::Color4u8 const& pixel = colorBitmap.GetPixel( x, y );
				if( pixel.g > 127 )
				{
					retVal.mCollisionMap.SetBit( x, y );
				}
				else
				{
					retVal.mCollisionMap.ClearBit( x, y );
				}
			}
		}
	}

	retVal.mTerrainTilesetPath = file::VFSPath::CreatePathFromString( desc.mTerrainTilesetPath );
	retVal.mTerrainTilemapPath = file::VFSPath::CreatePathFromString( desc.mTerrainTilemapPath );
	retVal.mCloud1TilesetPath = file::VFSPath::CreatePathFromString( desc.mCloud1TilesetPath );
	retVal.mCloud1TilemapPath = file::VFSPath::CreatePathFromString( desc.mCloud1TilemapPath );
	retVal.mCloud2TilesetPath = file::VFSPath::CreatePathFromString( desc.mCloud2TilesetPath );
	retVal.mCloud2TilemapPath = file::VFSPath::CreatePathFromString( desc.mCloud2TilemapPath );
	retVal.mCloud1ParallaxDelay = desc.mCloud1ParallaxDelay;
	retVal.mCloud2ParallaxDelay = desc.mCloud1ParallaxDelay;

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

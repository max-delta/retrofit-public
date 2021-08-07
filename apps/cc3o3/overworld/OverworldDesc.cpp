#include "stdafx.h"
#include "OverworldDesc.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"
#include "core_rftype/stl_extensions/vector.h"


RFTYPE_CREATE_META( RF::cc::overworld::AreaDesc )
{
	using RF::cc::overworld::AreaDesc;
	RFTYPE_META().ExtensionProperty( "mIdentifier", &AreaDesc::mIdentifier );
	RFTYPE_META().ExtensionProperty( "mType", &AreaDesc::mType );
	RFTYPE_META().RawProperty( "mX", &AreaDesc::mX );
	RFTYPE_META().RawProperty( "mY", &AreaDesc::mY );
	RFTYPE_META().RawProperty( "mWidth", &AreaDesc::mWidth );
	RFTYPE_META().RawProperty( "mHeight", &AreaDesc::mHeight );
	RFTYPE_META().RawProperty( "mFocusX", &AreaDesc::mFocusX );
	RFTYPE_META().RawProperty( "mFocusY", &AreaDesc::mFocusY );
	RFTYPE_REGISTER_BY_NAME( "AreaDesc" );
}

RFTYPE_CREATE_META( RF::cc::overworld::OverworldDesc )
{
	using RF::cc::overworld::OverworldDesc;
	RFTYPE_META().ExtensionProperty( "mCollisionMapPath", &OverworldDesc::mCollisionMapPath );
	RFTYPE_META().ExtensionProperty( "mTerrainTilesetPath", &OverworldDesc::mTerrainTilesetPath );
	RFTYPE_META().ExtensionProperty( "mTerrainTilemapPath", &OverworldDesc::mTerrainTilemapPath );
	RFTYPE_META().ExtensionProperty( "mCloud1TilesetPath", &OverworldDesc::mCloud1TilesetPath );
	RFTYPE_META().ExtensionProperty( "mCloud1TilemapPath", &OverworldDesc::mCloud1TilemapPath );
	RFTYPE_META().ExtensionProperty( "mCloud2TilesetPath", &OverworldDesc::mCloud2TilesetPath );
	RFTYPE_META().ExtensionProperty( "mCloud2TilemapPath", &OverworldDesc::mCloud2TilemapPath );
	RFTYPE_META().RawProperty( "mCloud1ParallaxDelay", &OverworldDesc::mCloud1ParallaxDelay );
	RFTYPE_META().RawProperty( "mCloud2ParallaxDelay", &OverworldDesc::mCloud2ParallaxDelay );
	RFTYPE_META().ExtensionProperty( "mAreas", &OverworldDesc::mAreas );
	RFTYPE_REGISTER_BY_NAME( "OverworldDesc" );
}

namespace RF::cc::overworld {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

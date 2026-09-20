#include "stdafx.h"
#include "SceneDesc.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"
#include "core_rftype/stl_extensions/vector.h"


RFTYPE_CREATE_META( RF::cc::scene::LayerDesc )
{
	using RF::cc::scene::LayerDesc;
	RFTYPE_META().ExtensionProperty( "mTileset", &LayerDesc::mTileset );
	RFTYPE_META().ExtensionProperty( "mTileLayer", &LayerDesc::mTileLayer );
	RFTYPE_META().ExtensionProperty( "mMode", &LayerDesc::mMode );
	RFTYPE_META().RawProperty( "mDepth", &LayerDesc::mDepth );
	RFTYPE_META().RawProperty( "mParallax", &LayerDesc::mParallax );
	RFTYPE_REGISTER_BY_NAME( "SceneLayerDesc" );
}

RFTYPE_CREATE_META( RF::cc::scene::SceneDesc )
{
	using RF::cc::scene::SceneDesc;
	RFTYPE_META().ExtensionProperty( "mLayers", &SceneDesc::mLayers );
	RFTYPE_REGISTER_BY_NAME( "SceneDesc" );
}

namespace RF::cc::scene {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

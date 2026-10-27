#pragma once
#include "GameNovel/scene/SceneFwd.h"

#include "rftl/string"
#include "rftl/vector"


namespace RF::scene {
///////////////////////////////////////////////////////////////////////////////

struct GAMENOVEL_API LayerDesc
{
	rftl::string mTileset;
	rftl::string mTileLayer;
	rftl::string mMode;
	int8_t mDepth = 0;
	uint8_t mParallax = 0;
};

///////////////////////////////////////////////////////////////////////////////

struct GAMENOVEL_API SceneDesc
{
	rftl::vector<LayerDesc> mLayers;
};

///////////////////////////////////////////////////////////////////////////////
}

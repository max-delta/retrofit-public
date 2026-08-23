#include "stdafx.h"
#include "Tileset.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

Tileset::Tileset()
{
	//
}



Tileset::~Tileset()
{
	// TODO: De-ref texture?
}



uint32_t Tileset::DebugGetTileWidth() const
{
	return mTileWidth;
}



uint32_t Tileset::DebugGetTileHeight() const
{
	return mTileHeight;
}

///////////////////////////////////////////////////////////////////////////////
}

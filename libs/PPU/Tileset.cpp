#include "stdafx.h"
#include "Tileset.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

Tileset::Tileset()
{
	//
}



Tileset::~Tileset()
{
	// TODO: De-ref texture?
}



uint32_t Tileset::DebugGetWidth() const
{
	return mTileWidth;
}



uint32_t Tileset::DebugGetHeight() const
{
	return mTileHeight;
}

///////////////////////////////////////////////////////////////////////////////
}}

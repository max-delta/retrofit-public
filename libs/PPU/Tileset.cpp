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



Tileset::TileAnim Tileset::GetTileAnim( TileIndex index ) const
{
	if( mTileAnims.size() <= index )
	{
		// Out of range, treat as unset
		return {};
	}

	return mTileAnims.at( index );
}



Tileset::TileAnim& Tileset::GetMutableTileAnim( TileAnims& tileAnims, TileIndex index )
{
	if( tileAnims.size() <= index )
	{
		// Out of range, grow to fit
		tileAnims.resize( index + 1u );
	}

	return tileAnims.at( index );
}

///////////////////////////////////////////////////////////////////////////////
}

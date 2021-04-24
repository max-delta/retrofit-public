#include "stdafx.h"
#include "TileLayer.h"

#include "core_math/math_casts.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

void TileLayer::Animate()
{
	mTimer.Animate( mLooping, mPaused );
}



size_t TileLayer::NumColumns() const
{
	return mNumColumns;
}



size_t TileLayer::NumRows() const
{
	return NumTiles() / NumColumns();
}



size_t TileLayer::NumTiles() const
{
	return mTiles.size();
}



TileLayer::Tile const& TileLayer::GetTile( size_t column, size_t row ) const
{
	return mTiles.at( NumColumns() * row + column );
}



TileLayer::Tile const& TileLayer::GetTile( size_t index ) const
{
	return mTiles.at( index );
}



TileLayer::Tile& TileLayer::GetMutableTile( size_t column, size_t row )
{
	return const_cast<Tile&>( GetTile( column, row ) );
}



TileLayer::Tile& TileLayer::GetMutableTile( size_t index )
{
	return const_cast<Tile&>( GetTile( index ) );
}



void TileLayer::GetTileZoomFactor( uint8_t& numer, uint8_t& denom ) const
{
	// User may have put a 0 in, we will count that as normal
	TileZoomFactor const zoomFactor = mTileZoomFactor != 0 ? mTileZoomFactor : kTileZoomFactor_Normal;

	// Exp=Factor-Normal
	// 2^(Exp)=Scale
	int16_t const exponent = zoomFactor - kTileZoomFactor_Normal;

	if( exponent == 0 )
	{
		numer = 1;
		denom = 1;
	}
	else if( exponent > 0 )
	{
		numer = math::integer_cast<uint8_t>( 1 << exponent );
		denom = 1;
	}
	else
	{
		RF_ASSERT( exponent < 0 );
		numer = 1;
		denom = math::integer_cast<uint8_t>( 1 << exponent );
	}
}



TileLayer::Tile const* TileLayer::Data() const
{
	return mTiles.data();
}



TileLayer::Tile* TileLayer::MutableData()
{
	return const_cast<TileLayer::Tile*>( Data() );
}



void TileLayer::Clear()
{
	size_t const size = NumTiles();
	mTiles.clear();
	mTiles.resize( size );
}



void TileLayer::ClearAndResize( size_t columns, size_t rows )
{
	mTiles.clear();
	mNumColumns = math::integer_cast<uint16_t>( columns );
	mTiles.resize( rows * columns );
}

///////////////////////////////////////////////////////////////////////////////
}

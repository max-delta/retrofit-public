#include "stdafx.h"
#include "Bitmap.h"

#include "core/rf_assert.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

Bitmap::Bitmap( ExplicitDefaultConstruct )
	: mScanlinelength( 0 )
{
	//
}



Bitmap::Bitmap( size_t width, size_t height )
	: mScanlinelength( width )
{
	RF_ASSERT( width > 0 );
	RF_ASSERT( height > 0 );
	mData.resize( mScanlinelength * height );
}



size_t Bitmap::GetWidth() const
{
	return mScanlinelength;
}



size_t Bitmap::GetHeight() const
{
	return mData.size() / mScanlinelength;
}



bool Bitmap::IsInBounds( size_t x, size_t y ) const
{
	return //
		x < GetWidth() &&
		y < GetHeight();
}



bool Bitmap::GetBit( size_t x, size_t y ) const
{
	size_t const index = y * mScanlinelength + x;
	return mData.at( index );
}



bool Bitmap::SetBit( size_t x, size_t y )
{
	size_t const index = y * mScanlinelength + x;
	Bits::reference ref = mData.at( index );
	bool const retVal = ref;
	ref = true;
	return retVal;
}



bool Bitmap::ClearBit( size_t x, size_t y )
{
	size_t const index = y * mScanlinelength + x;
	Bits::reference ref = mData.at( index );
	bool const retVal = ref;
	ref = false;
	return retVal;
}



bool Bitmap::FlipBit( size_t x, size_t y )
{
	size_t const index = y * mScanlinelength + x;
	Bits::reference ref = mData.at( index );
	bool const retVal = ref;
	ref = !retVal;
	return retVal;
}



size_t Bitmap::GetBitCount() const
{
	return mData.size();
}



size_t Bitmap::SetAll()
{
	for( size_t i = 0; i < mData.size(); i++ )
	{
		mData.at( i ) = true;
	}
	return GetBitCount();
}



size_t Bitmap::ClearAll()
{
	mData.clear();
	return GetBitCount();
}



size_t Bitmap::FlipAll()
{
	mData.flip();
	return GetBitCount();
}



Bitmap Bitmap::ExtractRegion( size_t x, size_t y, size_t width, size_t height ) const
{
	Bitmap retVal( width, height );
	for( size_t scanline = 0; scanline < height; scanline++ )
	{
		for( size_t pixel = 0; pixel < width; pixel++ )
		{
			bool const set = GetBit( x + pixel, y + scanline );
			if( set )
			{
				retVal.SetBit( pixel, scanline );
			}
			else
			{
				retVal.ClearBit( pixel, scanline );
			}
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

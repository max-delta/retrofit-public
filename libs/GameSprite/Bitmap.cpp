#include "stdafx.h"
#include "Bitmap.h"

#include "core_math/math_casts.h"


namespace RF::sprite {
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



Bitmap::Bitmap( Bitmap&& rhs )
	: RF_MOVE_CONSTRUCT( mScanlinelength )
	, RF_MOVE_CONSTRUCT( mData )
{
	//
}



Bitmap& Bitmap::operator=( Bitmap&& rhs )
{
	if( this != &rhs )
	{
		RF_MOVE_ASSIGN( mScanlinelength );
		RF_MOVE_ASSIGN( mData );
	}
	return *this;
}



size_t Bitmap::GetWidth() const
{
	return mScanlinelength;
}



size_t Bitmap::GetHeight() const
{
	return mData.size() / mScanlinelength;
}



Bitmap::ElementType const& Bitmap::GetPixel( size_t x, size_t y ) const
{
	size_t const index = y * mScanlinelength + x;
	return mData.at( index );
}



Bitmap::ElementType& Bitmap::GetMutablePixel( size_t x, size_t y )
{
	size_t const index = y * mScanlinelength + x;
	return mData.at( index );
}



Bitmap::ElementType const* Bitmap::GetData() const
{
	return mData.data();
}



size_t Bitmap::GetPixelCount() const
{
	return mData.size();
}



size_t Bitmap::FillAll( ElementType const& to )
{
	for( ElementType& pixel : mData )
	{
		pixel = to;
	}
	return GetPixelCount();
}



size_t Bitmap::ReplaceEach( ElementType const& from, ElementType const& to )
{
	size_t numReplaced = 0;
	for( ElementType& pixel : mData )
	{
		if( pixel == from )
		{
			pixel = to;
			numReplaced++;
		}
	}
	return numReplaced;
}



size_t Bitmap::ReplaceEachByAlpha( ElementType::ElementType const& alpha, ElementType const& to )
{
	size_t numReplaced = 0;
	for( ElementType& pixel : mData )
	{
		if( pixel.a == alpha )
		{
			pixel = to;
			numReplaced++;
		}
	}
	return numReplaced;
}



size_t Bitmap::ApplyStencilOverwrite( Bitmap const& source, ElementType::ElementType minAlpha )
{
	RF_ASSERT( GetWidth() == source.GetWidth() );
	RF_ASSERT( GetHeight() == source.GetHeight() );
	RF_ASSERT( GetPixelCount() == source.GetPixelCount() );
	size_t const numPixels = GetPixelCount();
	ElementType const* const srcData = source.GetData();

	size_t numOverwritten = 0;
	for( size_t i = 0; i < numPixels; i++ )
	{
		ElementType const& srcPixel = srcData[i];
		if( srcPixel.a >= minAlpha )
		{
			ElementType& destPixel = mData.at( i );
			destPixel = srcPixel;
			numOverwritten++;
		}
	}
	return numOverwritten;
}



size_t Bitmap::ApplyStencilOverwrite( Bitmap const& source, size_t x, size_t y, ElementType::ElementType minAlpha )
{
	size_t const targetXStart = x;
	size_t const xLen = source.GetWidth();
	size_t const targetYStart = y;
	size_t const yLen = source.GetHeight();
	RF_ASSERT( targetXStart < GetWidth() );
	RF_ASSERT( targetXStart + xLen <= GetWidth() );
	RF_ASSERT( targetYStart < GetHeight() );
	RF_ASSERT( targetYStart + yLen <= GetHeight() );

	size_t numOverwritten = 0;
	for( size_t ix = 0; ix < xLen; ix++ )
	{
		for( size_t iy = 0; iy < yLen; iy++ )
		{
			ElementType const& srcPixel = source.GetPixel( ix, iy );
			if( srcPixel.a >= minAlpha )
			{
				ElementType& destPixel = GetMutablePixel(
					targetXStart + ix,
					targetYStart + iy );
				destPixel = srcPixel;
				numOverwritten++;
			}
		}
	}
	return numOverwritten;
}



void Bitmap::ShiftUp( size_t distance, ElementType const& backfill )
{
	// HACK: Signed to make life easier
	int64_t const width = math::integer_cast<int64_t>( GetWidth() );
	int64_t const height = math::integer_cast<int64_t>( GetHeight() );
	int64_t const delta = math::integer_cast<int64_t>( distance );
	for( int64_t scanline = 0; scanline < height; scanline++ )
	{
		int64_t const& targetLine = scanline;
		int64_t const sourceLine = scanline + delta;

		if( sourceLine < 0 || sourceLine >= height )
		{
			// Out of bounds, back fill
			for( int64_t pixel = 0; pixel < width; pixel++ )
			{
				int64_t const& targetPixel = pixel;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) = backfill;
			}
		}
		else
		{
			// Copy
			for( int64_t pixel = 0; pixel < width; pixel++ )
			{
				int64_t const& targetPixel = pixel;
				int64_t const& sourcePixel = pixel;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) =
					GetPixel(
						math::integer_cast<size_t>( sourcePixel ),
						math::integer_cast<size_t>( sourceLine ) );
			}
		}
	}
}



void Bitmap::ShiftDown( size_t distance, ElementType const& backfill )
{
	// HACK: Signed to make life easier
	int64_t const width = math::integer_cast<int64_t>( GetWidth() );
	int64_t const height = math::integer_cast<int64_t>( GetHeight() );
	int64_t const delta = math::integer_cast<int64_t>( distance );
	for( int64_t scanline = height - 1; scanline >= 0; scanline-- )
	{
		int64_t const& targetLine = scanline;
		int64_t const sourceLine = scanline - delta;

		if( sourceLine < 0 || sourceLine >= height )
		{
			// Out of bounds, back fill
			for( int64_t pixel = 0; pixel < width; pixel++ )
			{
				int64_t const& targetPixel = pixel;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) = backfill;
			}
		}
		else
		{
			// Copy
			for( int64_t pixel = 0; pixel < width; pixel++ )
			{
				int64_t const& targetPixel = pixel;
				int64_t const& sourcePixel = pixel;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) =
					GetPixel(
						math::integer_cast<size_t>( sourcePixel ),
						math::integer_cast<size_t>( sourceLine ) );
			}
		}
	}
}



void Bitmap::ShiftLeft( size_t distance, ElementType const& backfill )
{
	// HACK: Signed to make life easier
	int64_t const width = math::integer_cast<int64_t>( GetWidth() );
	int64_t const height = math::integer_cast<int64_t>( GetHeight() );
	int64_t const delta = math::integer_cast<int64_t>( distance );
	for( int64_t pixel = 0; pixel < width; pixel++ )
	{
		int64_t const& targetPixel = pixel;
		int64_t const sourcePixel = pixel + delta;

		if( sourcePixel < 0 || sourcePixel >= width )
		{
			// Out of bounds, back fill
			for( int64_t scanline = 0; scanline < height; scanline++ )
			{
				int64_t const& targetLine = scanline;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) = backfill;
			}
		}
		else
		{
			// Copy
			for( int64_t scanline = 0; scanline < height; scanline++ )
			{
				int64_t const& targetLine = scanline;
				int64_t const& sourceLine = scanline;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) =
					GetPixel(
						math::integer_cast<size_t>( sourcePixel ),
						math::integer_cast<size_t>( sourceLine ) );
			}
		}
	}
}



void Bitmap::ShiftRight( size_t distance, ElementType const& backfill )
{
	// HACK: Signed to make life easier
	int64_t const width = math::integer_cast<int64_t>( GetWidth() );
	int64_t const height = math::integer_cast<int64_t>( GetHeight() );
	int64_t const delta = math::integer_cast<int64_t>( distance );
	for( int64_t pixel = width - 1; pixel >= 0; pixel-- )
	{
		int64_t const& targetPixel = pixel;
		int64_t const sourcePixel = pixel - delta;

		if( sourcePixel < 0 || sourcePixel >= width )
		{
			// Out of bounds, back fill
			for( int64_t scanline = 0; scanline < height; scanline++ )
			{
				int64_t const& targetLine = scanline;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) = backfill;
			}
		}
		else
		{
			// Copy
			for( int64_t scanline = 0; scanline < height; scanline++ )
			{
				int64_t const& targetLine = scanline;
				int64_t const& sourceLine = scanline;
				GetMutablePixel(
					math::integer_cast<size_t>( targetPixel ),
					math::integer_cast<size_t>( targetLine ) ) =
					GetPixel(
						math::integer_cast<size_t>( sourcePixel ),
						math::integer_cast<size_t>( sourceLine ) );
			}
		}
	}
}



Bitmap Bitmap::ExtractRegion( size_t x, size_t y, size_t width, size_t height ) const
{
	Bitmap retVal( width, height );
	for( size_t scanline = 0; scanline < height; scanline++ )
	{
		for( size_t pixel = 0; pixel < width; pixel++ )
		{
			retVal.GetMutablePixel( pixel, scanline ) = GetPixel( x + pixel, y + scanline );
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

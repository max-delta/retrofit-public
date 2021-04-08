#pragma once
#include "core/meta/ConstructorOverload.h"

#include "rftl/vector"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

class Bitmap
{
	//
	// Types and constants
private:
	using Bits = rftl::vector<bool>;


	//
	// Public methods
public:
	Bitmap() = delete;
	explicit Bitmap( ExplicitDefaultConstruct );
	Bitmap( size_t width, size_t height );

	size_t GetWidth() const;
	size_t GetHeight() const;

	// NOTE: Mutations return the previous value of the bit
	bool GetBit( size_t x, size_t y ) const;
	bool SetBit( size_t x, size_t y );
	bool ClearBit( size_t x, size_t y );
	bool FlipBit( size_t x, size_t y );

	size_t GetBitCount() const;

	size_t SetAll();
	size_t ClearAll();
	size_t FlipAll();

	// Create a new bitmap from a subset of the bitmap
	Bitmap ExtractRegion( size_t x, size_t y, size_t width, size_t height ) const;


	//
	// Private data
private:
	size_t mScanlinelength = 0;
	Bits mData;
};

///////////////////////////////////////////////////////////////////////////////
}

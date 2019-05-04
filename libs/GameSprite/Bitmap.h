#pragma once
#include "project.h"

#include "GameSprite/SpriteFwd.h"

#include "core_math/Color4u8.h"
#include "core/meta/ConstructorOverload.h"

#include "rftl/vector"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Data is stored as left-to-right scanlines in top-to-bottom order as a
//  contiguous buffer
class GAMESPRITE_API Bitmap
{
	RF_NO_COPY( Bitmap );

	//
	// Types
public:
	using ElementType = math::Color4u8;


	//
	// Public methods
public:
	Bitmap() = delete;
	explicit Bitmap( ExplicitDefaultConstruct );
	Bitmap( size_t width, size_t height );
	Bitmap( Bitmap&& rhs );

	Bitmap& operator=( Bitmap&& rhs );

	size_t GetWidth() const;
	size_t GetHeight() const;

	ElementType const& GetPixel( size_t x, size_t y ) const;
	ElementType& GetMutablePixel( size_t x, size_t y );

	ElementType const* GetData() const;
	size_t GetPixelCount() const;

	// All pixels become 'to'
	size_t FillAll( ElementType const& to );

	// All pixels that match 'from' become 'to'
	size_t ReplaceEach( ElementType const& from, ElementType const& to );

	// All pixels with an alpha that match 'alpha' become 'to'
	size_t ReplaceEachByAlpha( ElementType::ElementType const& alpha, ElementType const& to );

	// All pixels in 'source' that have atleast 'min' alpha perform overwrite
	size_t ApplyStencilOverwrite( Bitmap const& source, ElementType::ElementType minAlpha );

	// All pixels are shifted with truncation, and backfilled with 'backfill'
	void ShiftUp( size_t distance, ElementType const& backfill );
	void ShiftDown( size_t distance, ElementType const& backfill );
	void ShiftLeft( size_t distance, ElementType const& backfill );
	void ShiftRight( size_t distance, ElementType const& backfill );

	// Create a new bitmap from a subset of the bitmap
	Bitmap ExtractRegion( size_t x, size_t y, size_t width, size_t height ) const;


	//
	// Private data
private:
	size_t const mScanlinelength;
	rftl::vector<ElementType> mData;
};

///////////////////////////////////////////////////////////////////////////////
}}

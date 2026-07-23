#include "stdafx.h"
#include "TempImageDecoder.h"

#include "core_math/math_casts.h"

#include "stb_image/stb_image.h"


namespace RF::image {
///////////////////////////////////////////////////////////////////////////////

TempImageDecoder::TempImageDecoder( rftl::byte_view encodedBytes, uint8_t maxChannels )
{
	// Load using dubiously-typed API
	int x, y, n;
	unsigned char* const data = stbi_load_from_memory( reinterpret_cast<stbi_uc const*>( encodedBytes.data() ), math::integer_cast<int>( encodedBytes.size() ), &x, &y, &n, maxChannels );
	RF_ASSERT( data != nullptr );
	if( data == nullptr )
	{
		return;
	}
	RF_ASSERT( x > 0 );
	RF_ASSERT( y > 0 );
	RF_ASSERT( n > 0 );

	// Convert to more sensical types
	// NOTE: Per documentation 'n' is the original channels, not the actual
	//  decoded channels, which allocate to the requested number instead
	mData.mWidth = math::integer_cast<uint32_t>( x );
	mData.mHeight = math::integer_cast<uint32_t>( y );
	mData.mNumChannels = math::integer_cast<uint8_t>( maxChannels );
	mData.mBackingImplementationRawPointer = data;
}



TempImageDecoder::~TempImageDecoder()
{
	Discard();
}



uint32_t TempImageDecoder::GetWidth() const
{
	return mData.mWidth;
}



uint32_t TempImageDecoder::GetHeight() const
{
	return mData.mHeight;
}



uint8_t TempImageDecoder::GetNumChannels() const
{
	return mData.mNumChannels;
}



rftl::byte_view TempImageDecoder::GetDecodedBytes() const
{
	size_t const size = mData.mWidth * mData.mHeight * mData.mNumChannels;
	return rftl::byte_view( mData.mBackingImplementationRawPointer, size );
}



void TempImageDecoder::Discard()
{
	if( mData.mBackingImplementationRawPointer )
	{
		return;
	}

	// Free resource
	stbi_image_free( const_cast<void*>( mData.mBackingImplementationRawPointer ) );

	// Clear data
	mData = {};
}

///////////////////////////////////////////////////////////////////////////////
}

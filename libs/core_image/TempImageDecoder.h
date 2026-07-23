#pragma once

#include "core/macros.h"

#include "rftl/extension/byte_view.h"


namespace RF::image {
///////////////////////////////////////////////////////////////////////////////

// Temporarily decodes an image, requiring the caller to process the image data
//  to a longer-term storage location
// NOTE: It is possible to keep this object around longer using emplacement
//  tricks, but that is strongly discouraged
class TempImageDecoder
{
	RF_NO_COPY( TempImageDecoder );
	RF_NO_MOVE( TempImageDecoder );

	//
	// Public methods
public:
	TempImageDecoder() = delete;
	TempImageDecoder( rftl::byte_view encodedBytes, uint8_t maxChannels );
	~TempImageDecoder();

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint8_t GetNumChannels() const;
	rftl::byte_view GetDecodedBytes() const;

	void Discard();


	//
	// Private data
private:
	struct Data
	{
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		uint8_t mNumChannels = 0;
		void const* mBackingImplementationRawPointer = nullptr;
	} mData;
};

///////////////////////////////////////////////////////////////////////////////
}

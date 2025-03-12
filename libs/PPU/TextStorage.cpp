#include "stdafx.h"
#include "TextStorage.h"

#include "core_math/math_casts.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

template<size_t StorageSize>
void TextStorage<StorageSize>::Clear()
{
	mTextStorageUsed = 0;
}



template<size_t StorageSize>
uint16_t TextStorage<StorageSize>::GetNextWritableTextOffset() const
{
	RF_ASSERT( mTextStorageUsed <= kMaxTextStorage );
	if( mTextStorageUsed >= kMaxTextStorage )
	{
		// In the event the buffer is full, return the front of the buffer as
		//  a fallback
		return 0;
	}
	return mTextStorageUsed;
}



template<size_t StorageSize>
rftl::byte_span TextStorage<StorageSize>::GetMutableRemainingTextBuffer()
{
	// NOTE: Usage could equal the whole buffer, but it should never be more
	//  than that
	RF_ASSERT( mTextStorageUsed <= kMaxTextStorage );
	if( mTextStorageUsed >= kMaxTextStorage )
	{
		return {};
	}

	rftl::byte_span span = { mTextStorage.data(), mTextStorage.size() };
	span.remove_prefix( mTextStorageUsed );
	return span;
}



template<size_t StorageSize>
void TextStorage<StorageSize>::ConsumeTextBuffer( size_t bytesUsed )
{
	RF_ASSERT( bytesUsed < mTextStorage.size() );
	mTextStorageUsed += math::integer_cast<decltype( mTextStorageUsed )>( bytesUsed );
	RF_ASSERT( mTextStorageUsed <= kMaxTextStorage );
}



template<size_t StorageSize>
rftl::string_view TextStorage<StorageSize>::GetTextBufferForStringOffset( uint16_t offset ) const
{
	rftl::string_view const buffer = { mTextStorage.data(), mTextStorage.size() };
	return buffer.substr( offset );
}

///////////////////////////////////////////////////////////////////////////////
}

// Only allow these sizes for these known use-cases
template class PPU_API RF::gfx::ppu::TextStorage<RF::gfx::ppu::kMaxTextStorage>;
template class PPU_API RF::gfx::ppu::TextStorage<RF::gfx::ppu::kMaxDebugTextStorage>;

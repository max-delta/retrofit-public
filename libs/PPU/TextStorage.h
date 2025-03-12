#pragma once
#include "project.h"

#include "PPU/PPUConfig.h"

#include "core/macros.h"

#include "rftl/extension/byte_span.h"
#include "rftl/array"
#include "rftl/string_view"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

template<size_t StorageSize>
class TextStorage
{
	RF_NO_COPY( TextStorage );
	RF_NO_MOVE( TextStorage );


	//
	// Types and constants
public:
	static constexpr size_t kStorageSize = StorageSize;
	using UsedSizeType = uint16_t;
	static_assert( kStorageSize < rftl::numeric_limits<UsedSizeType>::max() );


	//
	// Public methods
public:
	TextStorage() = default;
	~TextStorage() = default;

	void Clear();

	uint16_t GetNextWritableTextOffset() const;
	rftl::byte_span GetMutableRemainingTextBuffer();
	void ConsumeTextBuffer( size_t bytesUsed );

	// NOTE: Buffer may be significantly larger than the actual stored text, be
	//  sure to look for the null-terminator
	rftl::string_view GetTextBufferForStringOffset( uint16_t offset ) const;


	//
	// Public data
public:
	UsedSizeType mTextStorageUsed;
	rftl::array<char, kStorageSize> mTextStorage;
};

///////////////////////////////////////////////////////////////////////////////
}

// Only allow these sizes for these known use-cases
extern template class RF::gfx::ppu::TextStorage<RF::gfx::ppu::kMaxTextStorage>;
extern template class RF::gfx::ppu::TextStorage<RF::gfx::ppu::kMaxDebugTextStorage>;
static_assert( sizeof( RF::gfx::ppu::TextStorage<RF::gfx::ppu::kMaxTextStorage> ) == RF::gfx::ppu::kMaxTextStorage + 2 );
static_assert( sizeof( RF::gfx::ppu::TextStorage<RF::gfx::ppu::kMaxDebugTextStorage> ) == RF::gfx::ppu::kMaxDebugTextStorage + 2 );

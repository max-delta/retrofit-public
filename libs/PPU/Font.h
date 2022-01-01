#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "core/macros.h"

#include "rftl/array"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Font
{
	RF_NO_COPY( Font );

private:
	friend class ppu::PPUController;
	friend class FontManager;


private:
	enum class SpacingMode : uint8_t
	{
		Invalid = 0,
		Fixed,
		Variable
	};
	using TileSize = uint8_t;
	static constexpr TileSize kInvalidTileSize = 0;
	// NOTE: Expect 16x16 grid of 256 characters
	static constexpr size_t kCharactersPerRow = 16;
	static constexpr size_t kCharactersPerColumn = 16;
	static constexpr size_t kNumCharacters = kCharactersPerRow * kCharactersPerColumn;


public:
	Font();
	Font( Font&& ) = default;
	~Font();
	Font& operator=( Font&& ) = default;
	DeviceFontID GetDeviceRepresentation() const;

	uint32_t DebugGetWidth() const;
	uint32_t DebugGetHeight() const;


private:
	friend class FontManager;
	file::FileBuffer mFileBuffer;
	SpacingMode mSpacingMode = SpacingMode::Invalid;
	DeviceFontID mDeviceRepresentation = kInvalidDeviceFontID;
	TileSize mTileWidth = kInvalidTileSize;
	TileSize mTileHeight = kInvalidTileSize;
	rftl::array<TileSize, 256> mVariableWidth;
	rftl::array<TileSize, 256> mVariableHeight;
};

///////////////////////////////////////////////////////////////////////////////
}

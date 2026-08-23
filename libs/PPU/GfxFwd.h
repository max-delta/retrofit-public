#pragma once
#include "project.h"

#include "core_math/ColorFwd.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

using DeviceTextureID = uint64_t;
inline constexpr DeviceTextureID kInvalidDeviceTextureID = 0;

using DeviceFontID = uint64_t;
inline constexpr DeviceFontID kInvalidDeviceFontID = 0;

using TimeSlowdownRate = uint8_t;
inline constexpr TimeSlowdownRate kTimeSlowdownRate_Normal = 1;
inline constexpr TimeSlowdownRate kTimeSlowdownRate_Half = 2;
inline constexpr TimeSlowdownRate kTimeSlowdownRate_Third = 3;
inline constexpr TimeSlowdownRate kTimeSlowdownRate_Quarter = 4;

using ManagedTextureID = uint64_t;
inline constexpr ManagedTextureID kInvalidManagedTextureID = 0;

using ManagedTilesetID = uint16_t;
inline constexpr ManagedTilesetID kInvalidManagedTilesetID = 0;

using ManagedFontID = uint16_t;
inline constexpr ManagedFontID kInvalidManagedFontID = 0;

using FrameID = uint64_t;
inline constexpr FrameID kInvalidFrameID = 0;
inline constexpr FrameID kFirstFrameID = 0;

using TileSize = uint8_t;
inline constexpr TileSize kInvalidTileSize = 0;

// Tiles are 10-bit indexed, with the upper limit denoted as the 'empty' tile,
//  under the expectation that most tilesets are only several hundred tiles in
//  the common cases
using TileIndex = uint16_t;
static constexpr TileIndex kEmptyTileIndex = 1023;
static constexpr TileIndex kMaxTileIndex = kEmptyTileIndex;
static_assert( kMaxTileIndex == 0b11'1111'1111 );

template<typename ColorT, size_t CountT>
class Palette;
template<size_t CountT>
using Palette4a5 = Palette<math::Color4a5, CountT>;
using Palette4a5_16 = Palette4a5<16>;

class Texture;
class Tileset;
class Font;

class TextureManager;
class TilesetManager;
class FontManager;

class DeviceInterface;

///////////////////////////////////////////////////////////////////////////////
}

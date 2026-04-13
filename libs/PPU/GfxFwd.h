#pragma once
#include "project.h"

#include "rftl/cstdint"


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

class Texture;
class Tileset;
class Font;

class TextureManager;
class TilesetManager;
class FontManager;

class DeviceInterface;

///////////////////////////////////////////////////////////////////////////////
}

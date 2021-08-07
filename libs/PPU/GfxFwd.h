#pragma once
#include "project.h"

#include "rftl/cstdint"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

using DeviceTextureID = uint64_t;
constexpr DeviceTextureID kInvalidDeviceTextureID = 0;

using DeviceFontID = uint64_t;
constexpr DeviceFontID kInvalidDeviceFontID = 0;

using TimeSlowdownRate = uint8_t;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Normal = 1;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Half = 2;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Third = 3;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Quarter = 4;

using ManagedTextureID = uint64_t;
constexpr ManagedTextureID kInvalidManagedTextureID = 0;

using ManagedTilesetID = uint16_t;
constexpr ManagedTilesetID kInvalidManagedTilesetID = 0;

using ManagedFontID = uint16_t;
constexpr ManagedFontID kInvalidManagedFontID = 0;

using FrameID = uint64_t;
constexpr FrameID kInvalidFrameID = 0;
constexpr FrameID kFirstFrameID = 0;

class Texture;
class Tileset;
class Font;

class TextureManager;
class TilesetManager;
class FontManager;

class DeviceInterface;

///////////////////////////////////////////////////////////////////////////////
}

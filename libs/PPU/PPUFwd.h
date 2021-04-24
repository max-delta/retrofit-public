#pragma once
#include "core_math/math_clamps.h"

#include "rftl/cstdint"

// Forwards
namespace RF::math {
template<typename T>
class Vector2;
}

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

static constexpr size_t kMaxObjects = 32;
static constexpr size_t kMaxTileLayers = 16;
static constexpr size_t kMaxStrings = 48;
static constexpr size_t kMaxStringLen = 71;

static constexpr size_t kMaxDebugStrings = 32;
static constexpr size_t kMaxDebugAuxStrings = 128;
static constexpr size_t kMaxDebugLines = 255;
static constexpr size_t kMaxDebugStringLen = 127;

using GameLayerID = uint8_t;
static constexpr size_t kNumGameLayers = 8;

using ManagedColliderID = uint64_t;
constexpr ManagedColliderID kInvalidManagedColliderID = 0;

using ManagedTextureID = uint64_t;
constexpr ManagedTextureID kInvalidManagedTextureID = 0;

using ManagedFramePackID = uint16_t;
constexpr ManagedFramePackID kInvalidManagedFramePackID = 0;

using ManagedTilesetID = uint16_t;
constexpr ManagedTilesetID kInvalidManagedTilesetID = 0;

using ManagedFontID = uint16_t;
constexpr ManagedFontID kInvalidManagedFontID = 0;

using FrameID = uint64_t;
constexpr FrameID kInvalidFrameID = 0;
constexpr FrameID kFirstFrameID = 0;

using GFXModifierHandlerID = uint64_t;
constexpr size_t kNumGFXModifiers = 8;

// Coordinates are +X=left, +Y=down
using PPUCoordElem = int16_t;
using PPUCoord = math::Vector2<PPUCoordElem>;
using PPUVec = math::Vector2<PPUCoordElem>;

using PPUTileElem = int8_t;
using PPUDepthLayer = int8_t;
constexpr PPUCoordElem kTileSize = 32;
constexpr PPUDepthLayer kFarthestLayer = 100;
constexpr PPUDepthLayer kNearestLayer = -100;
// At 32 tile size, here are some interesting tile ratios:
//  8x7 = SNES (Internal resolution)
//  8x7.5 = SNES output (Stretched pixels)
//  10x7 = Almost QVGA (4:3)
//  12x7 = Almost WQVGA (16:9 widescreen)
constexpr PPUCoordElem kDesiredWidth = kTileSize * 10;
constexpr PPUCoordElem kDesiredHeight = kTileSize * 7;
constexpr PPUTileElem kDesiredDiagonalTiles = static_cast<PPUTileElem>( math::Min( kDesiredWidth, kDesiredHeight ) / kTileSize );

using PPUZoomFactor = uint8_t;
constexpr PPUZoomFactor kInvalidZoomFactor = 0;

class Object;
class TileLayer;
class Texture;
class Tileset;
class Font;

class TextureManager;
class FramePackManager;
class TilesetManager;
class FontManager;

class Viewport;
class DeviceInterface;
class PPUController;

class FramePackBase;
template<size_t>
class FramePack;
using FramePack_4096 = FramePack<160>;
using FramePack_1024 = FramePack<37>;
using FramePack_512 = FramePack<16>;
using FramePack_256 = FramePack<6>;

///////////////////////////////////////////////////////////////////////////////
}

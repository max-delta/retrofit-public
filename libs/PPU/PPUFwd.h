#pragma once
#include "core_math/Vector2.h"
#include "core_math/math_clamps.h"

#include "rftl/cstdint"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

typedef uint64_t DeviceTextureID;
constexpr DeviceTextureID kInvalidDeviceTextureID = 0;

typedef uint64_t DeviceFontID;
constexpr DeviceFontID kInvalidDeviceFontID = 0;

typedef uint8_t TimeSlowdownRate;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Normal = 1;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Half = 2;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Third = 3;
static constexpr TimeSlowdownRate kTimeSlowdownRate_Quarter = 4;

typedef uint8_t GameLayerID;
static constexpr size_t kNumGameLayers = 8;

typedef uint64_t ManagedColliderID;
constexpr ManagedColliderID kInvalidManagedColliderID = 0;

typedef uint64_t ManagedTextureID;
constexpr ManagedTextureID kInvalidManagedTextureID = 0;

typedef uint16_t ManagedFramePackID;
constexpr ManagedFramePackID kInvalidManagedFramePackID = 0;

typedef uint16_t ManagedTilesetID;
constexpr ManagedTilesetID kInvalidManagedTilesetID = 0;

typedef uint16_t ManagedFontID;
constexpr ManagedFontID kInvalidManagedFontID = 0;

typedef uint64_t FrameID;
constexpr FrameID kInvalidFrameID = 0;
constexpr FrameID kFirstFrameID = 0;

typedef uint64_t GFXModifierHandlerID;
constexpr size_t kNumGFXModifiers = 8;

typedef int16_t PPUCoordElem;
typedef math::Vector2<PPUCoordElem> PPUCoord;
typedef int8_t PPUTileElem;
typedef int8_t PPUDepthLayer;
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

class Object;
class TileLayer;
class Texture;
class Tileset;
class Font;
class TextureManager;
class FramePackManager;
class TilesetManager;
class FontManager;
class DeviceInterface;

class FramePackBase;
template<size_t>
class FramePack;
typedef FramePack<160> FramePack_4096;
typedef FramePack<37> FramePack_1024;
typedef FramePack<16> FramePack_512;
typedef FramePack<6> FramePack_256;

///////////////////////////////////////////////////////////////////////////////
}}

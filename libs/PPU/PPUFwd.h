#pragma once
#include "core_math/Vector2.h"
#include "core_math/math_clamps.h"

#include <stdint.h>


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

typedef uint64_t DeviceTextureID;
constexpr DeviceTextureID k_InvalidDeviceTextureID = 0;

typedef uint8_t TimeSlowdownRate;
static constexpr TimeSlowdownRate k_TimeSlowdownRate_Normal = 1;
static constexpr TimeSlowdownRate k_TimeSlowdownRate_Half = 2;
static constexpr TimeSlowdownRate k_TimeSlowdownRate_Third = 3;
static constexpr TimeSlowdownRate k_TimeSlowdownRate_Quarter = 4;

typedef uint8_t GameLayerID;
static constexpr size_t k_NumGameLayers = 8;

typedef uint64_t ManagedTextureID;
constexpr ManagedTextureID k_InvalidManagedTextureID = 0;

typedef uint16_t ManagedFramePackID;
constexpr ManagedFramePackID k_InvalidManagedFramePackID = 0;

typedef uint64_t FrameID;
constexpr FrameID k_InvalidFrameID = 0;
constexpr FrameID k_FirstFrameID = 0;

typedef uint64_t GFXModifierHandlerID;
constexpr size_t k_NumGFXModifiers = 8;

typedef int16_t PPUCoordElem;
typedef math::Vector2<PPUCoordElem> PPUCoord;
typedef int8_t PPUTileElem;
typedef int8_t PPUDepthLayer;
constexpr PPUCoordElem k_TileSize = 32;
constexpr PPUDepthLayer k_FarthestLayer = 100;
constexpr PPUDepthLayer k_NearestLayer = -100;
// At 32 tile size, here are some interesting tile ratios:
//  8x7 = SNES (Internal resolution)
//  8x7.5 = SNES output (Stretched pixels)
//  10x7 = Almost QVGA (4:3)
//  12x7 = Almost WQVGA (16:9 widescreen)
constexpr PPUCoordElem k_DesiredWidth = k_TileSize * 10;
constexpr PPUCoordElem k_DesiredHeight = k_TileSize * 7;
constexpr PPUTileElem k_DesiredDiagonalTiles = math::Min( k_DesiredWidth, k_DesiredHeight ) / k_TileSize;

class Object;
class Texture;
class TextureManager;
class FramePackManager;
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

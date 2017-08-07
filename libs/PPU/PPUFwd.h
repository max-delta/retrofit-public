#pragma once
#include "core_math/Vector2.h"

#include <stdint.h>


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

typedef uint64_t TextureID;
constexpr TextureID k_InvalidTextureID = 0;

typedef uint64_t FrameID;
constexpr FrameID k_InvalidFrameID = 0;
constexpr FrameID k_FirstFrameID = 0;

typedef int16_t PPUCoordElem;
typedef math::Vector2<PPUCoordElem> PPUCoord;
typedef int8_t PPUTileElem;
typedef int8_t PPUDepthLayer;
constexpr PPUCoordElem k_TileSize = 32;
constexpr PPUDepthLayer k_FarthestLayer = 100;
constexpr PPUDepthLayer k_NearestLayer = -100;

class Object;
class Texture;
class TextureManager;
class DeviceInterface;

///////////////////////////////////////////////////////////////////////////////
}}

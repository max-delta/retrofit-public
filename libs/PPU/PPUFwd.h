#pragma once
#include "project.h"

#include "PPU/GfxFwd.h"


// Forwards
namespace RF::math {
template<typename T>
class Vector2;
template<typename T>
class AABB4;
}

namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

// Coordinates are +X=right, +Y=down (scanlines)
using CoordElem = int16_t;
using Coord = math::Vector2<CoordElem>;
using Vec2 = math::Vector2<CoordElem>;
using AABB = math::AABB4<CoordElem>;

// Coordinates are +Z=behind (right-handed)
using DepthLayer = int8_t;
constexpr DepthLayer kFarthestLayer = 100;
constexpr DepthLayer kNearestLayer = -100;

using TileElem = int8_t;
constexpr CoordElem kTileSize = 32;

using ZoomFactor = uint8_t;
constexpr ZoomFactor kInvalidZoomFactor = 0;

using GameLayerID = uint8_t;
static constexpr size_t kNumGameLayers = 8;

using GFXModifierHandlerID = uint64_t;
constexpr size_t kNumGFXModifiers = 8;

using ManagedColliderID = uint64_t;
constexpr ManagedColliderID kInvalidManagedColliderID = 0;

using ManagedFramePackID = uint16_t;
constexpr ManagedFramePackID kInvalidManagedFramePackID = 0;

class Object;
class TileLayer;
class Viewport;

template<size_t>
class TextStorage;

class FramePackBase;
template<size_t>
class FramePack;
using FramePack_4096 = FramePack<160>;
using FramePack_1024 = FramePack<37>;
using FramePack_512 = FramePack<16>;
using FramePack_256 = FramePack<6>;

class FramePackManager;

class PPUController;

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core_math/math_clamps.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

// At 32 tile size, here are some interesting tile ratios:
//  8x7 = SNES (Internal resolution)
//  8x7.5 = SNES output (Stretched pixels)
//  10x7 = Almost QVGA (4:3)
//  12x7 = Almost WQVGA (16:9 widescreen)
constexpr CoordElem kDesiredWidth = kTileSize * 10;
constexpr CoordElem kDesiredHeight = kTileSize * 7;
constexpr TileElem kDesiredDiagonalTiles = static_cast<TileElem>( math::Min( kDesiredWidth, kDesiredHeight ) / kTileSize );

inline constexpr size_t kMaxObjects = 32;
inline constexpr size_t kMaxTileLayers = 16;
inline constexpr size_t kMaxStrings = 48;
inline constexpr size_t kMaxStringLen = 128;
inline constexpr size_t kMaxTextStorage = 1366;

inline constexpr size_t kMaxDebugStrings = 32;
inline constexpr size_t kMaxDebugAuxStrings = 128;
inline constexpr size_t kMaxDebugLines = 255;
inline constexpr size_t kMaxDebugStringLen = 127;
inline constexpr size_t kMaxDebugTextStorage = 2048;

///////////////////////////////////////////////////////////////////////////////
}

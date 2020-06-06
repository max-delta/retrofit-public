#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

static constexpr char const kElementTilesetFullName[] = "elemgrid_16_128";
static constexpr gfx::PPUCoordElem kElementTileFullWidth = 128;
static constexpr gfx::PPUCoordElem kElementTileFullHeight = 16;

static constexpr char const kElementTilesetMiniName[] = "elemgrid_8_16";
static constexpr gfx::PPUCoordElem kElementTileMiniWidth = 16;
static constexpr gfx::PPUCoordElem kElementTileMiniHeight = 8;

static constexpr char const kElementTilesetMicroName[] = "elemgrid_2_4";
static constexpr gfx::PPUCoordElem kElementTileMicroWidth = 2;
static constexpr gfx::PPUCoordElem kElementTileMicroHeight = 4;


enum class ElementTilesetIndex : int8_t
{
	Empty = -1,

	Reserved = 0,
	LeftBorder,
	TopBorder,

	Unallocated,
	Hashed,
	Invalid,

	White,
	Grey,
	Black,

	DarkRed,
	DarkOrange,
	DarkYellow,

	DarkLime,
	DarkGreen,
	DarkTurqoise,

	DarkCyan,
	DarkSea,
	DarkBlue,

	DarkPurple,
	DarkPink,
	DarkMagenta,

	Red,
	Orange,
	Yellow,

	Lime,
	Green,
	Turqoise,

	Cyan,
	Sea,
	Blue,

	Purple,
	Pink,
	Magenta,
};

///////////////////////////////////////////////////////////////////////////////
}

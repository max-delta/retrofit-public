#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

static constexpr char const kElementTilesetFullName[] = "elemgrid_16_128";
static constexpr gfx::ppu::CoordElem kElementTileFullWidth = 128;
static constexpr gfx::ppu::CoordElem kElementTileFullHeight = 16;

static constexpr char const kElementTilesetMiniName[] = "elemgrid_8_16";
static constexpr gfx::ppu::CoordElem kElementTileMiniWidth = 16;
static constexpr gfx::ppu::CoordElem kElementTileMiniHeight = 8;

static constexpr char const kElementTilesetMicroName[] = "elemgrid_2_4";
static constexpr gfx::ppu::CoordElem kElementTileMicroWidth = 2;
static constexpr gfx::ppu::CoordElem kElementTileMicroHeight = 4;

static constexpr int8_t kElementTilesetOffsetDark = 1;
static constexpr int8_t kElementTilesetOffsetReserved = 2;

enum class ElementTilesetIndex : int8_t
{
	Empty = -1,

	TopLeftBorder = 0,
	LeftBorder,
	TopBorder,

	Unallocated,
	ReservedHashed,
	ReservedSolid,

	ElementStart,

	White = ElementStart,
	WhiteDark,
	WhiteReserved,

	Black,
	BlackDark,
	BlackReserved,

	Red,
	RedDark,
	RedReserved,

	Blue,
	BlueDark,
	BlueReserved,

	Yellow,
	YellowDark,
	YellowReserved,

	Green,
	GreenDark,
	GreenReserved,

	Material,
	MaterialDark,
	MaterialReserved,

	Purple,
	PurpleDark,
	PurpleReserved,

	Orange,
	OrangeDark,
	OrangeReserved,

	ElementEnd = OrangeReserved,
};

///////////////////////////////////////////////////////////////////////////////
}

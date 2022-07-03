#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

struct ElementTilesetDef
{
	char const* mName = nullptr;
	gfx::ppu::CoordElem mTileWidth = 0;
	gfx::ppu::CoordElem mTileHeight = 0;
	bool mUsesBorderSlots = false;
	bool mSupportsText = false;
};

static constexpr ElementTilesetDef kElementTilesetFull = {
	"elemgrid_16_128",
	128, 16,
	false,
	true };
static constexpr ElementTilesetDef kElementTilesetMedium = {
	"elemgrid_10_80",
	80, 10,
	false,
	true };
static constexpr ElementTilesetDef kElementTilesetMini = {
	"elemgrid_8_16",
	8, 16,
	false,
	false };
static constexpr ElementTilesetDef kElementTilesetMicro = {
	"elemgrid_2_4",
	4, 2,
	true,
	false };

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

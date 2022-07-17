#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"

#include "PPU/PPUFwd.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

enum class ElementTileSize : uint8_t
{
	Invalid = 0,
	Full,
	Medium,
	Mini,
	Micro
};

struct ElementTilesetDef
{
	ElementTileSize mSize = ElementTileSize::Invalid;
	char const* mName = nullptr;
	gfx::ppu::CoordElem mTileWidth = 0;
	gfx::ppu::CoordElem mTileHeight = 0;
	ui::FontPurposeID mFont = ui::kInvalidFontPurposeID;
	bool mUsesBorderSlots = false;
};

static constexpr ElementTilesetDef kElementTilesetFull = {
	ElementTileSize::Full,
	"elemgrid_16_128",
	128, 16,
	cc::ui::font::ElementLabelFull,
	false };
static constexpr ElementTilesetDef kElementTilesetMedium = {
	ElementTileSize::Medium,
	"elemgrid_10_80",
	80, 10,
	cc::ui::font::ElementLabelMedium,
	false };
static constexpr ElementTilesetDef kElementTilesetMini = {
	ElementTileSize::Mini,
	"elemgrid_8_16",
	16, 8,
	ui::kInvalidFontPurposeID,
	false };
static constexpr ElementTilesetDef kElementTilesetMicro = {
	ElementTileSize::Micro,
	"elemgrid_2_4",
	4, 2,
	ui::kInvalidFontPurposeID,
	true };

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

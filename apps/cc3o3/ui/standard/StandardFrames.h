#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"

#include "GameUI/BorderFrameShape.h"

#include "rftl/string_view"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

struct BorderFrameProto
{
	// NOTE: Text not stored, expected to be used with string literals
	rftl::string_view mResourceName = {};

	ui::BorderFrameShape mBorderShape = {};
};

struct BorderFrameDef
{
	// NOTE: Text not stored, expected to be used with string literals
	rftl::string_view mResourceName = {};

	gfx::ManagedTilesetID mManagedID = gfx::kInvalidManagedTilesetID;
	ui::BorderFrameShape mBorderShape = {};
};

// These require that the tileset is already loaded, and are expected to be
//  used for well-known resources
BorderFrameDef QueryBorderFrameDef(
	gfx::ppu::PPUController const& ppu,
	BorderFrameProto const& proto );
BorderFrameDef QueryBorderFrameDef(
	gfx::TilesetManager const& tsetMan,
	BorderFrameProto const& proto );

///////////////////////////////////////////////////////////////////////////////
namespace standard::frame {

namespace shape {
// Gives about 4 pixels of border, and about 40 pixels of repeating pattern,
//  with a granularity of 8 pixels, meaning the frame can't be any smaller than
//  a 16x16 square, since the corners won't be allowed to shrink smaller than
//  8 pixels
static constexpr ui::BorderFrameShape k4px = {
	.mExpectedTileDimensions{ 8, 8 },
	.mExpectedPatternDimensions{ 48, 48 },
	.mPaddingDimensions{ -4, -4 } };
}

static constexpr BorderFrameProto k4pxFlat1 = { "flat1_8_48", shape::k4px };
static constexpr BorderFrameProto k4pxFlat2 = { "flat2_8_48", shape::k4px };
static constexpr BorderFrameProto k4pxRetro1 = { "retro1_8_48", shape::k4px };
static constexpr BorderFrameProto k4pxRetro2 = { "retro2_8_48", shape::k4px };
static constexpr BorderFrameProto k4pxWood1 = { "wood_8_48", shape::k4px };

}
///////////////////////////////////////////////////////////////////////////////
namespace mockup::frame {

static constexpr BorderFrameProto k4pxTemplate = { "template_8_48", standard::frame::shape::k4px };

}
///////////////////////////////////////////////////////////////////////////////
}

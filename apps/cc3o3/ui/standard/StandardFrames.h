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

	BorderFrameShape mBorderShape = {};
};

struct BorderFrameDef
{
	// NOTE: Text not stored, expected to be used with string literals
	rftl::string_view mResourceName = {};

	gfx::ManagedTilesetID mManagedID = gfx::kInvalidManagedTilesetID;
	BorderFrameShape mBorderShape = {};
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
inline constexpr BorderFrameShape k4px = {
	.mExpectedTileDimensions{ 8, 8 },
	.mExpectedPatternDimensions{ 48, 48 },
	.mPaddingDimensions{ -4, -4 } };
}

inline constexpr BorderFrameProto k4pxFlat_Inactive = { "border4_flat_inactive", shape::k4px };
inline constexpr BorderFrameProto k4pxFlat_Active = { "border4_flat_active", shape::k4px };
inline constexpr BorderFrameProto k4pxDebug = { "border4_debug", shape::k4px };
inline constexpr BorderFrameProto k4pxPopup = { "border4_popup", shape::k4px };
inline constexpr BorderFrameProto k4pxNormal = { "border4_standard", shape::k4px };

}
///////////////////////////////////////////////////////////////////////////////
namespace mockup::frame {

inline constexpr BorderFrameProto k4pxTemplate = { "border4_template", standard::frame::shape::k4px };

}
///////////////////////////////////////////////////////////////////////////////
}

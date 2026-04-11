#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "rftl/string_view"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

struct FramePackProto
{
	// NOTE: Text not stored, expected to be used with string literals
	rftl::string_view mResourceName = {};

	gfx::ppu::CoordElem mExpectedWidth = 0;
	gfx::ppu::CoordElem mExpectedHeight = 0;
};

struct FramePackDef
{
	// NOTE: Text not stored, expected to be used with string literals
	rftl::string_view mResourceName = {};

	gfx::ppu::ManagedFramePackID mManagedID = gfx::ppu::kInvalidManagedFramePackID;
	uint8_t mMaxTimeIndex = 0;
	gfx::TimeSlowdownRate mPreferredSlowdownRate = 0;
	gfx::ppu::CoordElem mExpectedWidth = 0;
	gfx::ppu::CoordElem mExpectedHeight = 0;
};

// These require that the frame pack is already loaded, and are expected to be
//  used for well-known resources
FramePackDef QueryFramePackDef(
	gfx::ppu::PPUController const& ppu,
	FramePackProto const& proto );
FramePackDef QueryFramePackDef(
	gfx::ppu::FramePackManager const& framePackMan,
	FramePackProto const& proto );

///////////////////////////////////////////////////////////////////////////////
namespace standard {

static constexpr FramePackProto kTextTruncation = { "text_truncation", 16, 16 };
static constexpr FramePackProto kTextCompletion = { "text_completion", 16, 16 };

}
///////////////////////////////////////////////////////////////////////////////
namespace mockup {

static constexpr FramePackProto kWiggle64 = { "mock_wiggle_64", 64, 64 };
static constexpr FramePackProto kWiggle32 = { "mock_wiggle_32", 32, 32 };
static constexpr FramePackProto kWiggle16 = { "mock_wiggle_16", 16, 16 };
static constexpr FramePackProto kWiggle8 = { "mock_wiggle_8", 8, 8 };

}
///////////////////////////////////////////////////////////////////////////////
}

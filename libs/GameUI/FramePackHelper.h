#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/Object.h"

#include "core_math/Vector2.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

// Help manage the common state and calculations a frame pack is likely to need
//  when used in UI contexts
class GAMEUI_API FramePackHelper final
{
	RF_NO_COPY( FramePackHelper );

	//
	// Public methods
public:
	FramePackHelper() = default;

	void SetFramePack( gfx::ppu::ManagedFramePackID framePack, uint8_t maxTimeIndex, gfx::ppu::CoordElem expectedWidth, gfx::ppu::CoordElem expectedHeight );
	void SetSlowdown( gfx::TimeSlowdownRate rate );
	void SetJustification( Justification::Value justification );

	void ResetAnimationTimer();

	void Render( gfx::ppu::PPUController& renderer, gfx::ppu::AABB const& bounds, gfx::ppu::DepthLayer zLayer );

	//
	// Private data
private:
	gfx::ppu::Object mObject = {};
	gfx::ppu::Coord mExpectedDimensions = {};
	Justification::Value mJustification = Justification::TopLeft;
};

///////////////////////////////////////////////////////////////////////////////
}

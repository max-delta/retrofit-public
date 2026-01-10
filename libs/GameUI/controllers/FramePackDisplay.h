#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "PPU/Object.h"

#include "core_math/Vector2.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Displays a frame-pack
class GAMEUI_API FramePackDisplay final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( FramePackDisplay );

	//
	// Public methods
public:
	FramePackDisplay() = default;

	void SetFramePack( gfx::ppu::ManagedFramePackID framePack, uint8_t maxTimeIndex, gfx::ppu::CoordElem expectedWidth, gfx::ppu::CoordElem expectedHeight );
	void SetSlowdown( gfx::TimeSlowdownRate rate );
	void SetJustification( Justification::Value justification );

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	Justification::Value mJustification = Justification::TopLeft;
	gfx::ppu::Coord mExpectedDimensions = {};
	gfx::ppu::Object mObject = {};
};

///////////////////////////////////////////////////////////////////////////////
}

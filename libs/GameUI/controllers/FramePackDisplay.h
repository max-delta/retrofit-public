#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "PPU/Object.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API FramePackDisplay final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( FramePackDisplay );

	//
	// Public methods
public:
	FramePackDisplay() = default;

	void SetFramePack( gfx::ManagedFramePackID framePack, gfx::PPUCoordElem expectedWidth, gfx::PPUCoordElem expectedHeight );
	void SetJustification( Justification justification );

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	Justification mJustification = Justification::TopLeft;
	gfx::PPUCoord mExpectedDimensions = {};
	gfx::Object mObject = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}

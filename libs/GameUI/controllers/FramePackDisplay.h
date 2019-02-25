#pragma once
#include "project.h"

#include "GameUI/Controller.h"

#include "PPU/Object.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API FramePackDisplay final : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	void SetFramePack( gfx::ManagedFramePackID framePack, gfx::PPUCoordElem expectedWidth, gfx::PPUCoordElem expectedHeight );
	void SetJustification( Justification justification );

	virtual void OnRender( ContainerManager const& manager, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	Justification mJustification = Justification::TopLeft;
	gfx::PPUCoord mExpectedDimensions = {};
	gfx::Object mObject = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}

#pragma once
#include "project.h"

#include "GameNovel/ui/UIFwd.h"

#include "GameUI/controllers/InstancedController.h"

#include "PPU/TileLayer.h"


// Forwards
namespace RF::novel {
class CinematicDriver;
}

namespace RF::novel::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Canvas for novel to render the scene to
class GAMENOVEL_API SceneCanvas final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( SceneCanvas );

	//
	// Friends
public:
	friend class RF::novel::CinematicDriver;


	//
	// Public methods
public:
	SceneCanvas();

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;

	void ClearAll();


	//
	// Private data
private:
	gfx::ppu::TileLayer mTODOTileLayer = {};
};

///////////////////////////////////////////////////////////////////////////////
}

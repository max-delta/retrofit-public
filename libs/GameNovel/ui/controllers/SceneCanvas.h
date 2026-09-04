#pragma once
#include "project.h"

#include "GameNovel/ui/UIFwd.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF::novel::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Canvas for novel to render the scene to
class GAMENOVEL_API SceneCanvas final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( SceneCanvas );

	//
	// Public methods
public:
	SceneCanvas();

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	// TODO
};

///////////////////////////////////////////////////////////////////////////////
}

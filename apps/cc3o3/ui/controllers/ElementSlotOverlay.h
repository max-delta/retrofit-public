#pragma once
#include "project.h"

#include "cc3o3/ui/ElementSlotDisplayCache.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/InstancedController.h"

// Forwards
namespace RF::ui::controller {
class TextLabel;
}


namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Requires a specifically-sized container
class ElementSlotOverlay final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ElementSlotOverlay );

	//
	// Public methods
public:
	ElementSlotOverlay( ElementTileSize size );

	static gfx::ppu::Coord CalcContainerDimensions( ElementTileSize size );

	void UpdateFromCache( ElementSlotDisplayCache const& cache );

	void UpdateState( bool active, bool selected );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private methods
private:
	void UpdateDisplay();


	//
	// Private data
private:
	ElementTileSize const mSize = ElementTileSize::Invalid;
	WeakPtr<TextLabel> mNameLabel;
	ElementSlotDisplayCache mCache = {};
};

///////////////////////////////////////////////////////////////////////////////
}

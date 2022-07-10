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
	// Types and constants
public:
	enum class Size : uint8_t
	{
		Full = 0
	};


	//
	// Public methods
public:
	ElementSlotOverlay( Size size );

	static gfx::ppu::Coord CalcContainerDimensions( Size size );

	void UpdateFromCache( ElementSlotDisplayCache const& cache );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private methods
private:
	void UpdateDisplay();


	//
	// Private data
private:
	Size const mSize = Size::Full;
	WeakPtr<TextLabel> mNameLabel;
	ElementSlotDisplayCache mCache = {};
};

///////////////////////////////////////////////////////////////////////////////
}

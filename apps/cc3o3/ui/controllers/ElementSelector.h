#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/ui/ElementGridDisplayCache.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/GenericListBox.h"
#include "GameUI/controllers/TextLabel.h"

#include "PPU/TileLayer.h"


namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Requires a specifically-sized container
class ElementSelector final : public GenericListBox
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ElementSelector );

	//
	// Types and constants
public:
	static constexpr gfx::PPUCoordElem kContainerWidth = kElementTileFullWidth;
	static constexpr gfx::PPUCoordElem kContainerHeight = kElementTileFullHeight * character::kMaxSlotsPerElementLevel;


	//
	// Public methods
public:
	ElementSelector();

	void UpdateFromCharacter( state::ObjectRef const& character );
	void UpdateFromCache( ElementGridDisplayCache const& cache );

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private methods
private:
	virtual void PostInstanceAssign( UIContext& context, Container& container ) override;
	virtual bool ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const override;
	virtual bool OnUnhandledFocusEvent( UIContext& context, FocusEvent const& focusEvent ) override;

	WeakPtr<TextLabel> GetMutableSlotController( size_t slotIndex );

	void UpdateDisplay();


	//
	// Private data
private:
	size_t mLevel = 0;
	gfx::TileLayer mTileLayer = {};
	ElementGridDisplayCache mCache = {};
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameUI/FocusTree.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API FocusManager
{
	RF_NO_COPY( FocusManager );

	//
	// Public methods
public:
	FocusManager() = default;
	bool HandleEvent( UIContext& context, FocusEvent const& focusEvent ) const;

	FocusTree const& GetFocusTree() const;
	FocusTree& GetMutableFocusTree();

	void UpdateHardFocus( UIContext& context );
	void UpdateHardFocusSinglePass( UIContext& context, bool& causedChange );


	//
	// Private data
private:
	FocusTree mFocusTree;
	WeakPtr<FocusKey> mHardFocusKey;
};

///////////////////////////////////////////////////////////////////////////////
}}

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
	bool HandleEvent( FocusEvent const& focusEvent ) const;

	FocusTree const& GetFocusTree() const;
	FocusTree& GetMutableFocusTree();


	//
	// Private data
private:
	FocusTree mFocusTree;
};

///////////////////////////////////////////////////////////////////////////////
}}

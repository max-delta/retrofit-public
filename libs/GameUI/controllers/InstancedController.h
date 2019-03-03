#pragma once
#include "project.h"

#include "GameUI/Controller.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API InstancedController : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( InstancedController );

	//
	// Public methods
public:
	InstancedController();

	WeakPtr<FocusTreeNode> AddAsChildToFocusTreeNode( FocusManager& focusManager, FocusTreeNode const& parentNode );

	virtual void OnAddedToFocusTree( FocusManager& focusManager, FocusTreeNode const& newNode );
	virtual bool OnFocusEvent( FocusEvent const& focusEvent );


	//
	// Protected methods
protected:
	void InitializeFocus( Container& container );
	FocusTarget& GetFocusTarget();


	//
	// Private methods
private:
	static bool HandleFocusEvent( void* userData, FocusEvent const& focusEvent );


	//
	// Private data
private:
	UniquePtr<FocusTarget> mFocusTarget;
};

///////////////////////////////////////////////////////////////////////////////
}}}

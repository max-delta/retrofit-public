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

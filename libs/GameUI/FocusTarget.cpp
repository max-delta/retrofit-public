#include "stdafx.h"
#include "FocusTarget.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

bool FocusTarget::HandleEvent( UIContext& context, FocusEvent const& focusEvent ) const
{
	RF_ASSERT( mEventHandler != nullptr );
	return mEventHandler( context, mUserData, focusEvent );
}



bool FocusTarget::HasHardFocus() const
{
	return mHardFocusKey != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

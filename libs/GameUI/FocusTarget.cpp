#include "stdafx.h"
#include "FocusTarget.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

bool FocusTarget::HandleEvent( FocusEvent const& focusEvent ) const
{
	RF_ASSERT( mEventHandler != nullptr );
	return mEventHandler( mUserData, focusEvent );
}

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "InputEvent.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

InputEvent::InputEvent( time::CommonClock::time_point time, InputValue value )
	: mTime( time )
	, mValue( value )
{
	//
}



bool InputEvent::AreEqual( InputEvent const& lhs, InputEvent const& rhs )
{
	return //
		lhs.mTime == rhs.mTime &&
		lhs.mValue == rhs.mValue;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "Vitality.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void Vitality::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	mCurHealth.Bind( sharedWindow, parent.GetChild( "hp" ), mAlloc );
	mCurStamina.Bind( sharedWindow, parent.GetChild( "stamina" ), mAlloc );
}

///////////////////////////////////////////////////////////////////////////////
}

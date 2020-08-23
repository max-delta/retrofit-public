#include "stdafx.h"
#include "Combo.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void Combo::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	mComboMeter.Bind( sharedWindow, parent.GetChild( "combo", "attack" ), mAlloc );
	mComboTarget.Bind( sharedWindow, parent.GetChild( "combo", "target" ), mAlloc );
}

///////////////////////////////////////////////////////////////////////////////
}

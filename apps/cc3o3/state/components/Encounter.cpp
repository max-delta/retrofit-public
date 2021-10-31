#include "stdafx.h"
#include "Encounter.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void Encounter::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	for( size_t i = 0; i < mDeployed.size(); i++ )
	{
		mDeployed.at( i ).Bind( sharedWindow, parent.GetChild( "deployed", rftl::to_string( i ) ), mAlloc );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

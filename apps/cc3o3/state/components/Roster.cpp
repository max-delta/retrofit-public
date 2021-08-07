#include "stdafx.h"
#include "Roster.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void Roster::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	for( size_t i = 0; i < mEligible.size(); i++ )
	{
		mEligible.at( i ).Bind( sharedWindow, parent.GetChild( "eligible", rftl::to_string( i ) ), mAlloc );
	}

	for( size_t i = 0; i < mActiveTeam.size(); i++ )
	{
		mActiveTeam.at( i ).Bind( sharedWindow, parent.GetChild( "team", rftl::to_string( i ) ), mAlloc );
		mActiveTeam.at( i ) = company::kInvalidRosterIndex;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

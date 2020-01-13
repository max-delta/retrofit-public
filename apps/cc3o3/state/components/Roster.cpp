#include "stdafx.h"
#include "Roster.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void Roster::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	for( size_t i = 0; i < mEligible.size(); i++ )
	{
		rftl::string const indexAsString = ( rftl::stringstream() << math::integer_cast<size_t>( i ) ).str();
		mEligible.at( i ).Bind( sharedWindow, parent.GetChild( "eligible", indexAsString ), mAlloc );
	}

	for( size_t i = 0; i < mActiveTeam.size(); i++ )
	{
		rftl::string const indexAsString = ( rftl::stringstream() << math::integer_cast<size_t>( i ) ).str();
		mActiveTeam.at( i ).Bind( sharedWindow, parent.GetChild( "team", indexAsString ), mAlloc );
		mActiveTeam.at( i ) = kInvalidRosterIndex;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

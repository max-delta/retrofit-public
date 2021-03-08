#include "stdafx.h"
#include "RollbackFilters.h"

#include "Rollback/RollbackManager.h"
#include "Rollback/InputStreamRef.h"

#include "Logging/Logging.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

rollback::InputStreamRef RollbackFilters::GetMutableStreamRef(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamIdentifier const& identifier )
{
	rollback::InputStream& uncommitted = rollMan.GetMutableUncommittedStreams().at( identifier );
	rollback::InputStream const& committed = rollMan.GetCommittedStreams().at( identifier );
	return rollback::InputStreamRef( identifier, uncommitted, committed );
}



void RollbackFilters::PrepareLocalFrame(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamRef const& streamRef,
	time::CommonClock::time_point frameTime )
{
	streamRef.mUncommitted.increase_write_head( frameTime );
}



bool RollbackFilters::TryPrepareRemoteFrame(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamRef const& streamRef,
	time::CommonClock::time_point frameTime )
{
	using rollback::RollbackManager;

	if( frameTime < streamRef.mUncommitted.back().mTime )
	{
		RFLOG_WARNING(
			nullptr,
			RFCAT_GAMESYNC,
			"Failed to prepare a remote frame for input stream '%u' because it"
			" was too far in the past",
			streamRef.mIdentifier );
		return false;
	}

	if( frameTime < streamRef.mCommitted.back().mTime )
	{
		RFLOG_WARNING(
			nullptr,
			RFCAT_GAMESYNC,
			"Failed to prepare a remote frame for input stream '%u' because it"
			" would conflict with already committed data",
			streamRef.mIdentifier );
		return false;
	}

	streamRef.mUncommitted.increase_write_head( frameTime );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

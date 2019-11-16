#include "stdafx.h"
#include "RollbackFilters.h"

#include "Rollback/RollbackManager.h"

#include "Logging/Logging.h"


namespace RF { namespace sync {
///////////////////////////////////////////////////////////////////////////////

rollback::InputStreamRef RollbackFilters::GetMutableStreamRef(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamIdentifier const& identifier )
{
	rollback::InputStream& stream = rollMan.GetMutableUncommittedStreams().at( identifier );
	return rollback::InputStreamRef( identifier, stream );
}



void RollbackFilters::PrepareLocalFrame(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamRef const& streamRef,
	time::CommonClock::time_point frameTime )
{
	streamRef.second.increase_write_head( frameTime );
}



bool RollbackFilters::TryPrepareRemoteFrame(
	rollback::RollbackManager& rollMan,
	rollback::InputStreamRef const& streamRef,
	time::CommonClock::time_point frameTime )
{
	if( streamRef.second.back().mTime > frameTime )
	{
		RFLOG_WARNING(
			nullptr,
			RFCAT_GAMESYNC,
			"Failed to prepare a remote frame for input stream '%u' because it"
			" was too far in the past",
			streamRef.first );
		return false;
	}

	streamRef.second.increase_write_head( frameTime );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "ConnectionMananger.h"

#include "Logging/Logging.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

bool ConnectionManager::IsHostingASession() const
{
	ReaderLock const lock( mCommonMutex );

	bool const isHost = mAsHost.has_value();
	RF_ASSERT( ( isHost && mAsJoin.has_value() ) == false );
	return isHost;
}



void ConnectionManager::StartHostingASession( HostSpec spec )
{
	WriterLock const lock( mCommonMutex );

	RF_ASSERT( mAsHost.has_value() == false );
	RF_ASSERT( mAsJoin.has_value() == false );

	AsHost& host = mAsHost.emplace();
	host.mSpec = spec;
	RF_TODO_BREAK();
}



void ConnectionManager::StopHostingASession()
{
	WriterLock const lock( mCommonMutex );

	RF_ASSERT( mAsHost.has_value() );
	RF_ASSERT( mAsJoin.has_value() == false );

	RF_TODO_BREAK();
}



bool ConnectionManager::IsAcceptingASession() const
{
	ReaderLock const lock( mCommonMutex );

	bool const isJoin = mAsJoin.has_value();
	RF_ASSERT( ( isJoin && mAsHost.has_value() ) == false );
	return isJoin;
}



void ConnectionManager::StartAcceptingASession( JoinSpec spec )
{
	WriterLock const lock( mCommonMutex );

	RF_ASSERT( mAsHost.has_value() == false );
	RF_ASSERT( mAsJoin.has_value() == false );

	AsJoin& join = mAsJoin.emplace();
	join.mSpec = spec;
	RF_TODO_BREAK();
}



void ConnectionManager::StopAcceptingASession()
{
	WriterLock const lock( mCommonMutex );

	RF_ASSERT( mAsHost.has_value() == false );
	RF_ASSERT( mAsJoin.has_value() );

	RF_TODO_BREAK();
}

///////////////////////////////////////////////////////////////////////////////
}

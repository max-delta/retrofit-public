#include "stdafx.h"
#include "EndpointManager.h"

#include "core_platform/IncomingBufferStitcher.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/algorithms.h"


namespace RF::comm {
///////////////////////////////////////////////////////////////////////////////

WeakSharedPtr<IncomingStream> EndpointManager::AddIncomingStream( SharedPtr<IncomingStream>&& stream )
{
	WriterLock const lock( mCommonMutex );

	for( InStreams::value_type const& storedStream : mInStreams )
	{
		if( storedStream == stream )
		{
			RF_DBGFAIL();
			return nullptr;
		}
	}
	return mInStreams.emplace_back( rftl::move( stream ) );
}



WeakSharedPtr<OutgoingStream> EndpointManager::AddOutgoingStream( SharedPtr<OutgoingStream>&& stream )
{
	WriterLock const lock( mCommonMutex );

	for( OutStreams::value_type const& storedStream : mOutStreams )
	{
		if( storedStream == stream )
		{
			RF_DBGFAIL();
			return nullptr;
		}
	}
	return mOutStreams.emplace_back( rftl::move( stream ) );
}



SharedPtr<IncomingStream> EndpointManager::RemoveIncomingStream( WeakPtr<IncomingStream> const& stream )
{
	WriterLock const lock( mCommonMutex );

	InStreams::iterator iter;
	for( iter = mInStreams.begin(); iter != mInStreams.end(); iter++ )
	{
		if( *iter == stream )
		{
			break;
		}
	}

	SharedPtr<IncomingStream> retVal;
	if( iter != mInStreams.end() )
	{
		retVal = rftl::move( *iter );
		mInStreams.erase( iter );
	}
	return retVal;
}



SharedPtr<OutgoingStream> EndpointManager::RemoveOutgoingStream( WeakPtr<OutgoingStream> const& stream )
{
	WriterLock const lock( mCommonMutex );

	OutStreams::iterator iter;
	for( iter = mOutStreams.begin(); iter != mOutStreams.end(); iter++ )
	{
		if( *iter == stream )
		{
			break;
		}
	}

	SharedPtr<OutgoingStream> retVal;
	if( iter != mOutStreams.end() )
	{
		retVal = rftl::move( *iter );
		mOutStreams.erase( iter );
	}
	return retVal;
}



WeakSharedPtr<LogicalEndpoint> EndpointManager::AddEndpoint( EndpointIdentifier identifier )
{
	WriterLock const lock( mCommonMutex );
	LogicalEndpoints::const_iterator const iter = mLogicalEndpoints.find( identifier );
	if( iter == mLogicalEndpoints.end() )
	{
		SharedPtr<LogicalEndpoint>& newEndpoint = mLogicalEndpoints[identifier];
		newEndpoint = DefaultCreator<LogicalEndpoint>::Create();
		return newEndpoint;
	}
	return iter->second;
}



WeakSharedPtr<LogicalEndpoint> EndpointManager::GetEndpoint( EndpointIdentifier identifier )
{
	ReaderLock const lock( mCommonMutex );
	LogicalEndpoints::const_iterator const iter = mLogicalEndpoints.find( identifier );
	if( iter == mLogicalEndpoints.end() )
	{
		return nullptr;
	}
	RF_ASSERT( iter->second != nullptr );
	return iter->second;
}



void EndpointManager::RemoveEndpoint( EndpointIdentifier identifier )
{
	WriterLock const lock( mCommonMutex );
	mLogicalEndpoints.erase( identifier );
}



EndpointManager::EndpointIdentifiers EndpointManager::GetAllEndpoints() const
{
	ReaderLock const lock( mCommonMutex );

	EndpointIdentifiers retVal;
	for( LogicalEndpoints::value_type const& logicalEndpoint : mLogicalEndpoints )
	{
		retVal.emplace_back( logicalEndpoint.first );
	}
	return retVal;
}



void EndpointManager::RemoveAllEndpoints()
{
	WriterLock const lock( mCommonMutex );
	mLogicalEndpoints.clear();
}



void EndpointManager::RemoveOrphanedStreams( InStreams& incoming, OutStreams& outgoing )
{
	incoming = {};
	outgoing = {};

	WriterLock const lock( mCommonMutex );

	// Extract
	{
		// In
		for( InStreams::value_type& storedStream : mInStreams )
		{
			RF_ASSERT( storedStream != nullptr );
			bool hasStream = false;
			for( LogicalEndpoints::value_type const& endpoint : mLogicalEndpoints )
			{
				RF_ASSERT( endpoint.second != nullptr );
				if( endpoint.second->HasIncomingStream( storedStream ) )
				{
					hasStream = true;
					break;
				}
			}
			if( hasStream == false )
			{
				incoming.emplace_back( rftl::move( storedStream ) );
				RF_ASSERT( storedStream == nullptr );
			}
		}

		// Out
		for( OutStreams::value_type& storedStream : mOutStreams )
		{
			RF_ASSERT( storedStream != nullptr );
			bool hasStream = false;
			for( LogicalEndpoints::value_type const& endpoint : mLogicalEndpoints )
			{
				RF_ASSERT( endpoint.second != nullptr );
				if( endpoint.second->HasOutgoingStream( storedStream ) )
				{
					hasStream = true;
					break;
				}
			}
			if( hasStream == false )
			{
				outgoing.emplace_back( rftl::move( storedStream ) );
				RF_ASSERT( storedStream == nullptr );
			}
		}
	}

	// Clean
	{
		static constexpr auto isNull = []( auto const& value ) -> bool { return value == nullptr; };
		rftl::erase_if( mInStreams, isNull );
		rftl::erase_if( mOutStreams, isNull );
	}
}



void EndpointManager::RemoveOrphanedStreams( bool terminateOnRemoval )
{
	InStreams inStreams = {};
	OutStreams outStreams = {};
	RemoveOrphanedStreams( inStreams, outStreams );

	if( terminateOnRemoval )
	{
		for( SharedPtr<comm::IncomingStream>& stream : inStreams )
		{
			RF_ASSERT( stream != nullptr );
			stream->Terminate();
		}
		for( SharedPtr<comm::OutgoingStream>& stream : outStreams )
		{
			RF_ASSERT( stream != nullptr );
			stream->Terminate();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "SessionManager.h"

#include "Communication/EndpointManager.h"
#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

bool SessionManager::Connection::HasPartialHandshake() const
{
	return mInitialConnectionTime < mPartialHandshakeTime;
}



bool SessionManager::Connection::HasHandshake() const
{
	if( mInitialConnectionTime < mCompletedHandshakeTime )
	{
		RF_ASSERT( mPartialHandshakeTime <= mCompletedHandshakeTime );
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

SessionManager::SessionManager()
	: mEndpointManager( DefaultCreator<comm::EndpointManager>::Create() )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

void SessionManager::GetChannels(
	ConnectionIdentifier id,
	SharedPtr<comm::IncomingStream>& incoming,
	SharedPtr<comm::OutgoingStream>& outgoing )
{
	incoming = nullptr;
	outgoing = nullptr;

	comm::EndpointManager& endpointManager = *mEndpointManager;

	SharedPtr<comm::LogicalEndpoint> const endpointPtr = endpointManager.GetEndpoint( id ).Lock();
	if( endpointPtr == nullptr )
	{
		RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null endpoint" );
		return;
	}
	comm::LogicalEndpoint& endpoint = *endpointPtr;

	static constexpr comm::ChannelFlags::Value kDesiredFlags = comm::ChannelFlags::Ordered;
	WeakSharedPtr<comm::IncomingStream> incomingWPtr = nullptr;
	WeakSharedPtr<comm::OutgoingStream> outgoingWPtr = nullptr;
	endpoint.ChooseIncomingChannel( incomingWPtr, kDesiredFlags );
	endpoint.ChooseOutgoingChannel( outgoingWPtr, kDesiredFlags );
	SharedPtr<comm::IncomingStream> const incomingPtr = incomingWPtr.Lock();
	SharedPtr<comm::OutgoingStream> const outgoingPtr = outgoingWPtr.Lock();
	if( incomingPtr == nullptr || outgoingPtr == nullptr )
	{
		RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null channel" );
		return;
	}

	incoming = incomingPtr;
	outgoing = outgoingPtr;
}

///////////////////////////////////////////////////////////////////////////////
}

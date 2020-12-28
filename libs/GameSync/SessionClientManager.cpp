#include "stdafx.h"
#include "SessionClientManager.h"

#include "Communication/EndpointManager.h"
#include "Logging/Logging.h"

#include "PlatformNetwork_win32/TCPSocket.h"
#include "PlatformNetwork_win32/TCPBufferStream.h"

#include "core_platform/ThreadSettings.h"
#include "core_platform/IncomingBufferStitcher.h"

#include "core/ptr/default_creator.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

SessionClientManager::SessionClientManager( ClientSpec spec )
	: mSpec( spec )
	, mEndpointManager( DefaultCreator<comm::EndpointManager>::Create() )
{
	//
}



SessionClientManager::~SessionClientManager()
{
	if( IsReceivingASession() )
	{
		StopReceivingASession();
	}
}



bool SessionClientManager::IsReceivingASession() const
{
	ReaderLock const updateThreadLock( mUpdateThreadMutex );

	return mUpdateThread.IsStarted();
}



void SessionClientManager::StartReceivingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const updateThreadLock( mUpdateThreadMutex );

	if constexpr( config::kAsserts )
	{
		// Ensure there are no host connections
		{
			ReaderLock const connectionLock( mHostConnectionsMutex );

			RF_ASSERT( mHostConnections.empty() );
		}

		// Ensure there are no endpoints
		RF_ASSERT( mEndpointManager->GetAllEndpoints().empty() );
	}

	// Add our host connections
	{
		WriterLock const connectionLock( mHostConnectionsMutex );

		mHostConnections[kSingleHostIdentifier];
	}

	// Add our host endpoints
	mEndpointManager->AddEndpoint( kSingleHostIdentifier );

	// Initialize update thread
	{
		static constexpr auto prep = []() -> void //
		{
			using namespace platform::thread;
			SetThreadName( "Session Client Updater" );
			SetThreadPriority( ThreadPriority::Normal );
		};
		auto work = [this]() -> void //
		{
			this->ReceiveUpdate();
		};
		auto workCheck = [this]() -> bool //
		{
			// TODO: Add a throttling mechanism controlled by the update
			//  function, for fast error-loop cases (LAN unplugged, etc)
			RF_TODO_ANNOTATION( "Throttling logic" );
			return true;
		};
		auto termCheck = [this]() -> bool //
		{
			return mShouldReceiveASession.load( rftl::memory_order::memory_order_acquire ) == false;
		};
		mUpdateThread.Init( prep, work, workCheck, termCheck );
	}

	// Indicate the thread should run
	bool const wasReceiving = mShouldReceiveASession.exchange( true, rftl::memory_order::memory_order_acq_rel );
	RF_ASSERT( wasReceiving == false );

	RF_ASSERT( mUpdateThread.IsStarted() == false );
	mUpdateThread.Start();
}



void SessionClientManager::StopReceivingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const updateThreadLock( mUpdateThreadMutex );

	// Indicate our intent, which could potentially result in the update thread
	//  stopping on its own if we're extremely lucky
	bool const wasReceiving = mShouldReceiveASession.exchange( false, rftl::memory_order::memory_order_acq_rel );
	RF_ASSERT( wasReceiving );

	// Stop all host connections
	// NOTE: We have to do this, not the update thread, as it is likely
	//  blocked waiting for communications
	RF_TODO_ANNOTATION( "Stop all host connections" );

	// Stop update thread
	RF_ASSERT( mUpdateThread.IsStarted() );
	mUpdateThread.Stop();

	RF_TODO_ANNOTATION( "Destroy all host connections" );
	RF_TODO_ANNOTATION( "Destroy all endpoints" );
}

///////////////////////////////////////////////////////////////////////////////

void SessionClientManager::ReceiveUpdate()
{
	// Get the streams to communicate with a host
	// NOTE: In a multi-host scenario, another update will be responsible for
	//  the next host
	SharedPtr<comm::IncomingStream> incomingStream = nullptr;
	SharedPtr<comm::OutgoingStream> outgoingStream = nullptr;
	GetOrCreateNextUpdateChannels( incomingStream, outgoingStream );
	if( incomingStream == nullptr || outgoingStream == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Failed to create/select a host connection as client" );
		return;
	}

	// TODO:
	//  * Check how long since we've sent data
	//   * If never, tell the host we're legit and need a session data feed
	//  * Check if host has sent us new data
	//   * If so, process it
	RF_TODO_BREAK();
}



void SessionClientManager::GetOrCreateNextUpdateChannels( SharedPtr<comm::IncomingStream>& incomingStream, SharedPtr<comm::OutgoingStream>& outgoingStream )
{
	comm::EndpointManager& endpointManager = *mEndpointManager;
	RF_ASSERT( endpointManager.GetAllEndpoints().empty() == false );

	// NOTE: In a multi-host scenario, we need more complex logic to
	//  evaluate what hosts we have, when we've heard from them, and which
	//  we're missing but could possibly connect to (and haven't failed to
	//  connect to them recently)
	SharedPtr<comm::LogicalEndpoint> const singleHost = endpointManager.GetEndpoint( kSingleHostIdentifier ).Lock();
	RF_ASSERT( singleHost != nullptr );
	static constexpr comm::ChannelFlags::Value kDesiredFlags = comm::ChannelFlags::Ordered;
	auto const selectChannels = [&incomingStream, &outgoingStream]( comm::LogicalEndpoint& endpoint ) -> bool //
	{
		WeakSharedPtr<comm::IncomingStream> attemptIncoming = nullptr;
		WeakSharedPtr<comm::OutgoingStream> attemptOutgoing = nullptr;
		endpoint.ChooseIncomingChannel( attemptIncoming, kDesiredFlags );
		endpoint.ChooseOutgoingChannel( attemptOutgoing, kDesiredFlags );
		incomingStream = attemptIncoming.Lock();
		outgoingStream = attemptOutgoing.Lock();
		if( incomingStream == nullptr || outgoingStream == nullptr )
		{
			RF_ASSERT( incomingStream == nullptr );
			RF_ASSERT( outgoingStream == nullptr );
			return false;
		}
		return true;
	};

	// Select the best available streams
	bool const foundExistingStreams = selectChannels( *singleHost );
	if( foundExistingStreams == false )
	{
		// Failed to select streams, attempt to make new ones
		CreateHostChannels( kSingleHostIdentifier, mSpec );
		bool const foundNewStreams = selectChannels( *singleHost );
		if( foundNewStreams == false )
		{
			RF_TODO_ANNOTATION( "Update timings for throttling logic" );
		}
	}
}



void SessionClientManager::CreateHostChannels( comm::EndpointIdentifier hostIdentifier, ClientSpec spec )
{
	using TCPSocket = platform::network::TCPSocket;

	comm::EndpointManager& endpointManager = *mEndpointManager;

	// Remove all existing channels by resetting endpoint
	endpointManager.RemoveEndpoint( hostIdentifier );
	endpointManager.AddEndpoint( hostIdentifier );

	// Clear out and terminate any orphans this may have created
	endpointManager.RemoveOrphanedStreams( true );

	// Reset connection data
	{
		WriterLock const connectionLock( mHostConnectionsMutex );

		mHostConnections.at( kSingleHostIdentifier ) = Connection{};
	}

	// Block for new connection
	// NOTE: Not expected to block for an unreasonable time, but may need to
	//  re-visit with the addition of a timeout parameter on the API
	SharedPtr<TCPSocket> newConnection = DefaultCreator<TCPSocket>::Create(
		TCPSocket::ConnectClientSocket( spec.mHostname, spec.mPort ) );
	if( newConnection->IsValid() == false )
	{
		// Failure may be intermittent, and may succeed if tried again later
		RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Failed to form a new host connection as client" );
		return;
	}

	// Add new streams to manager
	WeakSharedPtr<comm::IncomingStream> incomingStream;
	WeakSharedPtr<comm::OutgoingStream> outgoingStream;
	{
		using namespace platform;
		using namespace platform::network;
		UniquePtr<TCPIncomingBufferStream> incomingTCPStream = DefaultCreator<TCPIncomingBufferStream>::Create( newConnection );
		SharedPtr<TCPOutgoingBufferStream> outgoingTCPStream = DefaultCreator<TCPOutgoingBufferStream>::Create( newConnection );
		SharedPtr<IncomingBufferStitcher> incomingStitcher = DefaultCreator<IncomingBufferStitcher>::Create( std::move( incomingTCPStream ) );
		incomingStream = endpointManager.AddIncomingStream( std::move( incomingStitcher ) );
		outgoingStream = endpointManager.AddOutgoingStream( std::move( outgoingTCPStream ) );
	}

	// Add new channels to endpoint
	SharedPtr<comm::LogicalEndpoint> const hostEndpoint = endpointManager.GetEndpoint( hostIdentifier ).Lock();
	RF_ASSERT( hostEndpoint != nullptr );
	hostEndpoint->AddIncomingChannel( incomingStream, {} );
	hostEndpoint->AddOutgoingChannel( outgoingStream, {} );
}

///////////////////////////////////////////////////////////////////////////////
}

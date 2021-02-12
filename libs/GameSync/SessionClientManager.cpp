#include "stdafx.h"
#include "SessionClientManager.h"

#include "GameSync/protocol/Logic.h"
#include "GameSync/protocol/Standards.h"

#include "Communication/EndpointManager.h"
#include "Logging/Logging.h"

#include "PlatformNetwork_win32/TCPSocket.h"
#include "PlatformNetwork_win32/TCPBufferStream.h"

#include "core_platform/ThreadSettings.h"
#include "core_platform/IncomingBufferStitcher.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/ptr_transform.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

bool SessionClientManager::Connection::HasPartialHandshake() const
{
	return mInitialConnectionTime < mOutgoingHandshakeTime;
}



bool SessionClientManager::Connection::HasHandshake() const
{
	if( mInitialConnectionTime < mCompletedHandshakeTime )
	{
		RF_ASSERT( mOutgoingHandshakeTime < mCompletedHandshakeTime );
		return true;
	}
	return false;
}

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
	return mHandshakeThread.IsStarted();
}



void SessionClientManager::StartReceivingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const startStopLock( mStartStopMutex );

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

	// Initialize handshake thread
	{
		static constexpr auto prep = []() -> void //
		{
			using namespace platform::thread;
			SetThreadName( "Session Client Handshaker" );
			SetThreadPriority( ThreadPriority::Normal );
		};
		auto work = [this]() -> void //
		{
			this->DoHandshakes();
		};
		auto workCheck = [this]() -> bool //
		{
			bool const wasUneventful = mLastHandshakeUneventful.exchange( false, rftl::memory_order::memory_order_acquire );
			if( wasUneventful )
			{
				return false;
			}
			return true;
		};
		auto termCheck = [this]() -> bool //
		{
			return mShouldReceiveASession.load( rftl::memory_order::memory_order_acquire ) == false;
		};
		mHandshakeThread.Init( prep, work, workCheck, termCheck );
		mHandshakeThread.SetSafetyWakeupInterval( kHandshakeThrottle );
	}

	// Indicate the thread should run
	bool const wasReceiving = mShouldReceiveASession.exchange( true, rftl::memory_order::memory_order_acq_rel );
	RF_ASSERT( wasReceiving == false );

	RF_ASSERT( mHandshakeThread.IsStarted() == false );
	mHandshakeThread.Start();
}



void SessionClientManager::StopReceivingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const startStopLock( mStartStopMutex );

	// Indicate our intent, which could potentially result in the handshake
	//  thread stopping on its own if we're extremely lucky
	bool const wasReceiving = mShouldReceiveASession.exchange( false, rftl::memory_order::memory_order_acq_rel );
	RF_ASSERT( wasReceiving );

	// Stop all host connections
	// NOTE: We have to do this, not the update thread, as it is likely
	//  blocked waiting for communications
	RF_TODO_ANNOTATION( "Stop all host connections" );

	// Stop handshake thread
	RF_ASSERT( mHandshakeThread.IsStarted() );
	mHandshakeThread.Stop();

	RF_TODO_ANNOTATION( "Destroy all host connections" );
	RF_TODO_ANNOTATION( "Destroy all endpoints" );
}



bool SessionClientManager::HasPendingOperations() const
{
	size_t bytesIncoming = 0;

	ReaderLock const connectionLock( mHostConnectionsMutex );

	comm::EndpointManager& endpointManager = *mEndpointManager;
	for( Connections::value_type const& connection : mHostConnections )
	{
		ConnectionIdentifier const& id = connection.first;
		Connection const& conn = connection.second;
		if( conn.HasHandshake() == false )
		{
			continue;
		}

		SharedPtr<comm::LogicalEndpoint> const endpointPtr = endpointManager.GetEndpoint( id ).Lock();
		if( endpointPtr == nullptr )
		{
			RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null endpoint in operation check" );
			continue;
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
			RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null channel in operation check" );
			continue;
		}
		comm::IncomingStream& incoming = *incomingPtr;
		comm::OutgoingStream& outgoing = *outgoingPtr;

		if( incoming.IsTerminated() || outgoing.IsTerminated() )
		{
			RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Terminated stream in operation check" );
			continue;
		}

		size_t const incomingSize = incoming.PeekNextBufferSize();
		if( incomingSize == 0 )
		{
			// No data yet
			continue;
		}

		bytesIncoming += incomingSize;
	}

	return bytesIncoming > 0;
}



SessionClientManager::Diagnostics SessionClientManager::ReportDiagnostics() const
{
	Diagnostics retVal = {};

	{
		ReaderLock const connectionLock( mHostConnectionsMutex );

		for( Connections::value_type const& hostConnection : mHostConnections )
		{
			Connection const& conn = hostConnection.second;
			if( conn.mInitialConnectionTime < conn.mCompletedHandshakeTime )
			{
				retVal.mValidConnections++;
			}
			else
			{
				retVal.mInvalidConnections++;
			}
		}
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

void SessionClientManager::DoHandshakes()
{
	Clock::time_point const now = Clock::now();

	// NOTE: In a multi-host scenario, this will need to be refactored into a
	//  looping concept, similar to the host manager and client connections

	// Get the streams to communicate with a host
	SharedPtr<comm::IncomingStream> incomingStream = nullptr;
	SharedPtr<comm::OutgoingStream> outgoingStream = nullptr;
	GetOrCreateNextHandshakeChannels( incomingStream, outgoingStream );
	if( incomingStream == nullptr || outgoingStream == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Failed to create/select a host connection as client" );
		return;
	}
	comm::IncomingStream& incoming = *incomingStream;
	comm::OutgoingStream& outgoing = *outgoingStream;

	// Check if we can probably ignore it before taking a write lock
	{
		ReaderLock const connectionLock( mHostConnectionsMutex );

		Connection& hostConnection = mHostConnections.at( kSingleHostIdentifier );
		RF_ASSERT( Clock::kLowest < hostConnection.mInitialConnectionTime );
		if( hostConnection.HasHandshake() )
		{
			// Already has handshake
			mLastHandshakeUneventful.store( true, rftl::memory_order::memory_order_release );
			return;
		}
	}

	// Check for, and perform any needed handshakes
	{
		WriterLock const connectionLock( mHostConnectionsMutex );

		Connection& hostConnection = mHostConnections.at( kSingleHostIdentifier );

		RF_ASSERT( Clock::kLowest < hostConnection.mInitialConnectionTime );
		if( hostConnection.HasPartialHandshake() == false )
		{
			// Never attempted handshake, need to do that

			protocol::EncryptionState& attemptedEncryption = hostConnection.mEncryption;
			attemptedEncryption.mPending = protocol::kExpectedEncryption;

			protocol::PrepareEncryptionRequest( attemptedEncryption );
			protocol::Buffer hello = protocol::CreateHelloTransmission( protocol::kMaxRecommendedTransmissionSize, attemptedEncryption );
			outgoing.StoreNextBuffer( rftl::move( hello ) );
			hostConnection.mOutgoingHandshakeTime = now;
		}

		if( hostConnection.HasHandshake() == false )
		{
			// Never completed handshake, need to do that

			size_t const incomingSize = incoming.PeekNextBufferSize();
			if( incomingSize == 0 )
			{
				// No data yet
				mLastHandshakeUneventful.store( true, rftl::memory_order::memory_order_release );
				return;
			}

			// If the handshake succeeds, we expect to enable encryption
			protocol::EncryptionState& attemptedEncryption = hostConnection.mEncryption;

			// See if data is valid
			protocol::Buffer const& buffer = incoming.CloneNextBuffer();
			rftl::byte_view bytes( buffer.begin(), buffer.end() );
			protocol::ReadResult const result = protocol::TryDecodeWelcomeTransmission( bytes, attemptedEncryption );
			if( result == protocol::ReadResult::kSuccess )
			{
				if( attemptedEncryption.mPending != protocol::kExpectedEncryption )
				{
					// Unsupported encryption, kill the connection
					RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Welcome transmission specified unsupported encryption, terminating connection" );
					incoming.Terminate();
					return;
				}

				// Success, consume the bytes
				RFLOG_MILESTONE( nullptr, RFCAT_GAMESYNC, "Welcome transmission was valid, upgrading connection" );
				size_t const consumedBytes = buffer.size() - bytes.size();
				protocol::Buffer const discarded = incoming.FetchNextBuffer( consumedBytes );

				// Finish handshake
				protocol::ApplyPendingEncryption( attemptedEncryption );
				hostConnection.mCompletedHandshakeTime = now;
			}
			else if( result == protocol::ReadResult::kTooSmall )
			{
				// Not enough data yet, attempt a stitch and try again next pass
				RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Welcome transmission wasn't large enough to decode. Suspicious..." );
				incoming.TryStitchNextBuffer();
				return;
			}
			else
			{
				// Bad transmission, kill the connection
				RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Welcome transmission was invalid, terminating connection" );
				incoming.Terminate();
				return;
			}
		}
	}
}



void SessionClientManager::GetOrCreateNextHandshakeChannels( SharedPtr<comm::IncomingStream>& incomingStream, SharedPtr<comm::OutgoingStream>& outgoingStream )
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
		FormHostConnection( kSingleHostIdentifier, mSpec );
		bool const foundNewStreams = selectChannels( *singleHost );
		if( foundNewStreams == false )
		{
			RF_TODO_ANNOTATION( "Update timings for throttling logic" );
		}
	}
}



void SessionClientManager::FormHostConnection( comm::EndpointIdentifier hostIdentifier, ClientSpec spec )
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
	TCPSocket newConnection = TCPSocket::ConnectClientSocket( spec.mHostname, spec.mPort );
	if( newConnection.IsValid() == false )
	{
		// Failure may be intermittent, and may succeed if tried again later
		RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Failed to form a new host connection as client" );
		return;
	}

	// Update connection data
	{
		WriterLock const connectionLock( mHostConnectionsMutex );

		mHostConnections.at( kSingleHostIdentifier ).mInitialConnectionTime = Clock::now();
	}

	// Create channels
	CreateHostChannels( hostIdentifier, DefaultCreator<TCPSocket>::Create( std::move( newConnection ) ) );

	// Wake the handshaker to check out the new connection
	mHandshakeThread.Wake();
}



void SessionClientManager::CreateHostChannels( comm::EndpointIdentifier hostIdentifier, UniquePtr<platform::network::TCPSocket>&& newConnection )
{
	comm::EndpointManager& endpointManager = *mEndpointManager;

	// Add new streams to manager
	WeakSharedPtr<comm::IncomingStream> incomingStream;
	WeakSharedPtr<comm::OutgoingStream> outgoingStream;
	{
		using namespace platform;
		using namespace platform::network;
		SharedPtr<TCPSocket> sharedSocket;
		PtrTransformer<TCPSocket>::PerformTransformation( rftl::move( newConnection ), sharedSocket );
		UniquePtr<TCPIncomingBufferStream> incomingTCPStream = DefaultCreator<TCPIncomingBufferStream>::Create( sharedSocket );
		SharedPtr<TCPOutgoingBufferStream> outgoingTCPStream = DefaultCreator<TCPOutgoingBufferStream>::Create( sharedSocket );
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

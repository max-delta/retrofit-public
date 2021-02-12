#include "stdafx.h"
#include "SessionHostManager.h"

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

#include "rftl/extension/static_vector.h"
#include "rftl/extension/algorithms.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

bool SessionHostManager::Connection::HasHandshake() const
{
	return mInitialConnectionTime < mIncomingHandshakeTime;
}

///////////////////////////////////////////////////////////////////////////////

SessionHostManager::SessionHostManager( HostSpec spec )
	: mSpec( spec )
	, mEndpointManager( DefaultCreator<comm::EndpointManager>::Create() )
{
	//
}



SessionHostManager::~SessionHostManager()
{
	if( IsHostingASession() )
	{
		StopHostingASession();
	}
}



bool SessionHostManager::IsHostingASession() const
{
	return mListenerThread.IsStarted();
}



void SessionHostManager::StartHostingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const startStopLock( mStartStopMutex );

	if constexpr( config::kAsserts )
	{
		// Ensure there are no client connections
		{
			ReaderLock const connectionLock( mClientConnectionsMutex );

			RF_ASSERT( mClientConnections.empty() );
		}

		// Ensure there are no endpoints
		RF_ASSERT( mEndpointManager->GetAllEndpoints().empty() );
	}

	// Create listener socket
	{
		WriterLock const listenSocketLock( mListenerSocketMutex );

		RF_ASSERT( mListenerSocket == nullptr );
		mListenerSocket = DefaultCreator<platform::network::TCPSocket>::Create(
			platform::network::TCPSocket::BindServerSocket( mSpec.mIPv6, mSpec.mDevLoopback, mSpec.mPort ) );
		if( mListenerSocket->IsValid() == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESYNC,
				"Failed to bind a listener socket, hosted session will not be"
				" able to accept new clients. See logs for details." );
		}
	}

	// Initialize listener thread
	{
		static constexpr auto prep = []() -> void //
		{
			using namespace platform::thread;
			SetThreadName( "Session Host Listener" );
			SetThreadPriority( ThreadPriority::Normal );
		};
		auto work = [this]() -> void //
		{
			this->AcceptNewConnection();
		};
		auto workCheck = [this]() -> bool //
		{
			ReaderLock const lock( this->mListenerSocketMutex );
			RF_ASSERT( this->mListenerSocket != nullptr );
			return this->mListenerSocket->IsValid();
		};
		auto termCheck = [this]() -> bool //
		{
			ReaderLock const lock( this->mListenerSocketMutex );
			RF_ASSERT( this->mListenerSocket != nullptr );
			return this->mListenerSocket->IsValid() == false;
		};
		mListenerThread.Init( prep, work, workCheck, termCheck );
	}

	// Initialize validator thread
	{
		static constexpr auto prep = []() -> void //
		{
			using namespace platform::thread;
			SetThreadName( "Session Host Validator" );
			SetThreadPriority( ThreadPriority::Normal );
		};
		auto work = [this]() -> void //
		{
			this->ValidateUntrustedConnections();
		};
		auto workCheck = [this]() -> bool //
		{
			bool const wasUneventful = mLastValidationUneventful.exchange( false, rftl::memory_order::memory_order_acquire );
			if( wasUneventful )
			{
				return false;
			}
			return true;
		};
		mValidatorThread.Init( prep, work, workCheck, nullptr );
		mValidatorThread.SetSafetyWakeupInterval( kHandshakeThrottle );
	}

	// Start threads
	RF_ASSERT( mListenerThread.IsStarted() == false );
	mListenerThread.Start();
	RF_ASSERT( mValidatorThread.IsStarted() == false );
	mValidatorThread.Start();
}



void SessionHostManager::StopHostingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const startStopLock( mStartStopMutex );

	// Stop listener socket
	// NOTE: We have to do this, not the listener thread, as it is likely
	//  blocked waiting for a new connection
	{
		ReaderLock const listenSocketLock( mListenerSocketMutex );

		RF_ASSERT( mListenerSocket != nullptr );
		mListenerSocket->Shutdown();
	}

	// Stop validator thread
	RF_ASSERT( mValidatorThread.IsStarted() );
	mValidatorThread.Stop();

	// Stop listener thread
	RF_ASSERT( mListenerThread.IsStarted() );
	mListenerThread.Stop();

	// Destroy listener socket
	{
		WriterLock const listenSocketLock( mListenerSocketMutex );

		RF_ASSERT( mListenerSocket != nullptr );
		mListenerSocket = nullptr;
	}

	RF_TODO_ANNOTATION( "Stop all client connections" );
	RF_TODO_ANNOTATION( "Destroy all client connections" );
	RF_TODO_ANNOTATION( "Destroy all endpoints" );
}



bool SessionHostManager::HasPendingOperations() const
{
	size_t bytesIncoming = 0;
	size_t terminatedConnections = 0;

	ReaderLock const connectionLock( mClientConnectionsMutex );

	comm::EndpointManager& endpointManager = *mEndpointManager;
	for( Connections::value_type const& connection : mClientConnections )
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
			terminatedConnections++;
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

	return //
		bytesIncoming > 0 ||
		terminatedConnections > 0 ||
		mRecentConnectionChanges.load( rftl::memory_order::memory_order_acquire );
}



SessionHostManager::Diagnostics SessionHostManager::ReportDiagnostics() const
{
	Diagnostics retVal = {};

	{
		ReaderLock const connectionLock( mClientConnectionsMutex );

		for( Connections::value_type const& clientConnection : mClientConnections )
		{
			Connection const& conn = clientConnection.second;
			if( conn.HasHandshake() )
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

void SessionHostManager::AcceptNewConnection()
{
	using TCPSocket = platform::network::TCPSocket;

	// Block for new connection
	// NOTE: Stopping of the session will need to interrupt this
	TCPSocket newConnection;
	{
		ReaderLock const listenSocketLock( mListenerSocketMutex );

		RF_ASSERT( mListenerSocket != nullptr );
		RF_ASSERT( mListenerSocket->IsListener() );
		newConnection = TCPSocket::WaitForNewClientConnection( *mListenerSocket );
	}

	// Accepting a connection may fail due to intermittent reasons, which
	//  should result in a retry, or a more persistent problem such as a
	//  socket shutdown (like is caused by a session stop)
	if( newConnection.IsValid() == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Failed to accept a new client connection as host" );
		return;
	}

	// Generate a new ID and store the connection
	ConnectionIdentifier newID = ConnectionIDGen::kInvalid;
	{
		WriterLock const connectionLock( mClientConnectionsMutex );

		if( mClientConnections.size() >= kMaxConnectionCount )
		{
			// Drop the connection
			RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Max connection count exceeded, had to drop a new connection" );
			return;
		}

		newID = mConnectionIdentifierGen.Generate();

		RF_ASSERT( mClientConnections.count( newID ) == 0 );
		Connection& conn = mClientConnections[newID];
		conn.mInitialConnectionTime = Clock::now();
	}

	// Create channels
	CreateClientChannels( newID, DefaultCreator<TCPSocket>::Create( std::move( newConnection ) ) );

	// Wake the validator to check out the new connection
	mValidatorThread.Wake();
}



void SessionHostManager::CreateClientChannels( comm::EndpointIdentifier clientIdentifier, UniquePtr<platform::network::TCPSocket>&& newConnection )
{
	comm::EndpointManager& endpointManager = *mEndpointManager;

	RF_ASSERT( endpointManager.GetEndpoint( clientIdentifier ).Weaken() == nullptr );

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
	SharedPtr<comm::LogicalEndpoint> const clientEndpoint = endpointManager.AddEndpoint( clientIdentifier ).Lock();
	RF_ASSERT( clientEndpoint != nullptr );
	clientEndpoint->AddIncomingChannel( incomingStream, {} );
	clientEndpoint->AddOutgoingChannel( outgoingStream, {} );
}



void SessionHostManager::ValidateUntrustedConnections()
{
	Clock::time_point const now = Clock::now();

	struct UntrustedConnection
	{
		ConnectionIdentifier mIdentifier = {};
		Clock::time_point mInitialConnectionTime = Clock::kLowest;
		protocol::EncryptionState mEncryption = {};
		bool shouldTrust = false;
		bool shouldDestroy = false;
	};

	// Figure out which connections to check
	rftl::static_vector<UntrustedConnection, kMaxConnectionCount> untrustedConnections;
	{
		ReaderLock const connectionLock( mClientConnectionsMutex );

		RF_ASSERT( mClientConnections.size() <= kMaxConnectionCount );
		for( Connections::value_type const& clientConnection : mClientConnections )
		{
			ConnectionIdentifier const& id = clientConnection.first;
			Connection const& conn = clientConnection.second;

			if( conn.HasHandshake() == false )
			{
				// No valid data yet
				UntrustedConnection untrustedConnection = {};
				untrustedConnection.mIdentifier = id;
				untrustedConnection.mInitialConnectionTime = conn.mInitialConnectionTime;
				untrustedConnection.mEncryption = conn.mEncryption;
				untrustedConnections.emplace_back( rftl::move( untrustedConnection ) );
			}
		}
	}
	if( untrustedConnections.empty() )
	{
		mLastValidationUneventful.store( true, rftl::memory_order::memory_order_release );
		return;
	}

	// Check each identifier
	comm::EndpointManager& endpointManager = *mEndpointManager;
	for( UntrustedConnection& untrusted : untrustedConnections )
	{
		ConnectionIdentifier const& id = untrusted.mIdentifier;

		SharedPtr<comm::LogicalEndpoint> const endpointPtr = endpointManager.GetEndpoint( id ).Lock();
		if( endpointPtr == nullptr )
		{
			// We weren't holding a lock on the connections, so it's possible
			//  this was recently terminated or still being setup
			RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null endpoint in untrusted check" );
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
			// We weren't holding a lock on the connections, so it's possible
			//  this was recently terminated or still being setup
			RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null channel in untrusted check" );
			continue;
		}
		comm::IncomingStream& incoming = *incomingPtr;
		comm::OutgoingStream& outgoing = *outgoingPtr;

		if( incoming.IsTerminated() )
		{
			// Terminated, needs to be removed
			untrusted.shouldDestroy = true;
			continue;
		}

		Clock::duration const timePassed = now - untrusted.mInitialConnectionTime;
		RF_ASSERT( timePassed >= Clock::duration{ 0 } );
		if( timePassed > protocol::kRecommendedHandshakeTimeout )
		{
			// Timed out, kill the connection and let it get cleaned up
			RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Untrusted connection has timed out, terminating connection" );
			incoming.Terminate();
			continue;
		}

		size_t const incomingSize = incoming.PeekNextBufferSize();
		if( incomingSize == 0 )
		{
			// No data yet
			continue;
		}

		// If the handshake succeeds, we expect to enable encryption
		protocol::EncryptionState& attemptedEncryption = untrusted.mEncryption;

		// See if data is valid
		protocol::Buffer const& buffer = incoming.CloneNextBuffer();
		rftl::byte_view bytes( buffer.begin(), buffer.end() );
		protocol::ReadResult const result = protocol::TryDecodeHelloTransmission( bytes, attemptedEncryption );
		if( result == protocol::ReadResult::kSuccess )
		{
			if( attemptedEncryption.mPending != protocol::kExpectedEncryption )
			{
				// Unsupported encryption, kill the connection and let it get
				//  cleaned up
				RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Hello transmission requested unsupported encryption, terminating connection" );
				incoming.Terminate();
				continue;
			}

			// Success, consume the bytes
			RFLOG_MILESTONE( nullptr, RFCAT_GAMESYNC, "Hello transmission was valid, upgrading connection" );
			size_t const consumedBytes = buffer.size() - bytes.size();
			protocol::Buffer const discarded = incoming.FetchNextBuffer( consumedBytes );

			// Flag the identifier for upgrade
			untrusted.shouldTrust = true;

			// Finish handshake
			protocol::PrepareEncryptionResponse( attemptedEncryption );
			protocol::Buffer welcome = protocol::CreateWelcomeTransmission(
				protocol::kMaxRecommendedTransmissionSize,
				attemptedEncryption );
			outgoing.StoreNextBuffer( rftl::move( welcome ) );
			protocol::ApplyPendingEncryption( attemptedEncryption );
		}
		else if( result == protocol::ReadResult::kTooSmall )
		{
			// Not enough data yet, attempt a stitch and try again next pass
			RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Hello transmission wasn't large enough to decode. Suspicious..." );
			incoming.TryStitchNextBuffer();
			continue;
		}
		else
		{
			// Bad transmission, kill the connection and let it get cleaned up
			RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Hello transmission was invalid, terminating connection" );
			incoming.Terminate();
			continue;
		}
	}

	// Clean out unchanged connections
	{
		static constexpr auto isUnchanged = []( UntrustedConnection const& untrusted ) -> bool //
		{
			RF_ASSERT( ( untrusted.shouldTrust && untrusted.shouldDestroy ) == false );
			return untrusted.shouldTrust == false && untrusted.shouldDestroy == false;
		};
		rftl::erase_if( untrustedConnections, isUnchanged );
	}
	if( untrustedConnections.empty() )
	{
		mLastValidationUneventful.store( true, rftl::memory_order::memory_order_release );
		return;
	}

	// Update the connection data
	{
		WriterLock const connectionLock( mClientConnectionsMutex );

		RF_ASSERT( mClientConnections.size() <= kMaxConnectionCount );
		for( UntrustedConnection const& untrusted : untrustedConnections )
		{
			RF_ASSERT( untrusted.shouldTrust || untrusted.shouldDestroy );
			ConnectionIdentifier const& id = untrusted.mIdentifier;

			Connections::iterator const iter = mClientConnections.find( id );
			if( iter == mClientConnections.end() )
			{
				// We dropped the lock on the connections, so it's possible
				//  this was terminated during processing
				RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Missing connection in untrusted check" );
				continue;
			}

			if( untrusted.shouldDestroy )
			{
				mClientConnections.erase( iter );
			}
			else if( untrusted.shouldTrust )
			{
				// Apply encryption
				iter->second.mEncryption = untrusted.mEncryption;

				// Make valid
				RF_ASSERT( Clock::kLowest < iter->second.mInitialConnectionTime );
				RF_ASSERT( iter->second.mInitialConnectionTime < now );
				iter->second.mIncomingHandshakeTime = now;
				RF_ASSERT( iter->second.HasHandshake() );
			}
			else
			{
				RF_DBGFAIL();
			}
		}
	}

	// Need to send a connection list, an update to existing connections as
	//  well as an initial list for the newly trusted connections
	// NOTE: There is a theoretical race condition where we can get here
	//  by changing the trust on only connections that were terminated while
	//  we were inspecting them, but the superfluous connection update we send
	//  should be benign

	RFLOG_INFO( nullptr, RFCAT_GAMESYNC, "Trust additions performed, need to send a connection update" );
	mRecentConnectionChanges.store( true, rftl::memory_order::memory_order_release );
}

///////////////////////////////////////////////////////////////////////////////
}

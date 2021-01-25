#include "stdafx.h"
#include "SessionHostManager.h"

#include "Communication/EndpointManager.h"
#include "Logging/Logging.h"

#include "PlatformNetwork_win32/TCPSocket.h"
#include "PlatformNetwork_win32/TCPBufferStream.h"

#include "core_platform/ThreadSettings.h"
#include "core_platform/IncomingBufferStitcher.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/ptr_transform.h"

#include "rftl/extension/static_vector.h"


namespace RF::sync {
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
	ReaderLock const listenThreadLock( mListenerThreadMutex );

	return mListenerThread.IsStarted();
}



void SessionHostManager::StartHostingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const listenThreadLock( mListenerThreadMutex );

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
		// NOTE: Lock taken at start of function

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
		WriterLock const validatorThreadLock( mValidatorThreadMutex );

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
			// TODO: Add a throttling mechanism controlled by the validate
			//  function, for dealing for potentially malicious clients or
			//  waiting for buggy/slow clients
			RF_TODO_ANNOTATION( "Throttling logic" );
			( (void)this );
			return true;
		};
		mValidatorThread.Init( prep, work, workCheck, nullptr );
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
	WriterLock const listenThreadLock( mListenerThreadMutex );

	// Stop listener socket
	// NOTE: We have to do this, not the listener thread, as it is likely
	//  blocked waiting for a new connection
	{
		ReaderLock const listenSocketLock( mListenerSocketMutex );

		RF_ASSERT( mListenerSocket != nullptr );
		mListenerSocket->Shutdown();
	}

	// Stop validator thread
	{
		RF_ASSERT( mValidatorThread.IsStarted() );
		mValidatorThread.Stop();
	}

	// Stop listener thread
	{
		// NOTE: Lock taken at start of function

		RF_ASSERT( mListenerThread.IsStarted() );
		mListenerThread.Stop();
	}

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
	// Figure out which connections to check
	rftl::static_vector<ConnectionIdentifier, kMaxConnectionCount> untrustedIdentifiers;
	{
		ReaderLock const connectionLock( mClientConnectionsMutex );

		RF_ASSERT( mClientConnections.size() <= kMaxConnectionCount );
		for( Connections::value_type const& clientConnection : mClientConnections )
		{
			ConnectionIdentifier const& id = clientConnection.first;
			Connection const& conn = clientConnection.second;

			if( conn.mLatestValidInboundData < conn.mInitialConnectionTime )
			{
				// No valid data yet
				untrustedIdentifiers.emplace_back( id );
			}
		}
	}
	if( untrustedIdentifiers.empty() )
	{
		return;
	}

	// Check each identifier
	rftl::static_vector<ConnectionIdentifier, kMaxConnectionCount> newlyTrustedIdentifiers;
	comm::EndpointManager& endpointManager = *mEndpointManager;
	for( ConnectionIdentifier const& id : untrustedIdentifiers )
	{
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
		endpoint.ChooseIncomingChannel( incomingWPtr, kDesiredFlags );
		SharedPtr<comm::IncomingStream> const incomingPtr = incomingWPtr.Lock();
		if( incomingPtr == nullptr )
		{
			// We weren't holding a lock on the connections, so it's possible
			//  this was recently terminated or still being setup
			RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null channel in untrusted check" );
			continue;
		}
		comm::IncomingStream& incoming = *incomingPtr;

		size_t const incomingSize = incoming.PeekNextBufferSize();
		if( incomingSize == 0 )
		{
			// No data yet
			continue;
		}

		// TODO: See if data is valid
		RF_TODO_BREAK();
		bool dataIsValid = false;
		if( dataIsValid )
		{
			newlyTrustedIdentifiers.emplace_back( id );
		}
	}
	if( newlyTrustedIdentifiers.empty() )
	{
		return;
	}

	// Update the connection times
	bool validConnectionsChanged = false;
	{
		WriterLock const connectionLock( mClientConnectionsMutex );

		Clock::time_point const now = Clock::now();

		RF_ASSERT( mClientConnections.size() <= kMaxConnectionCount );
		for( ConnectionIdentifier const& id : newlyTrustedIdentifiers )
		{
			Connections::iterator const iter = mClientConnections.find( id );
			if( iter == mClientConnections.end() )
			{
				// We dropped the lock on the connections, so it's possible
				//  this was terminated during processing
				RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Missing connection in untrusted check" );
				continue;
			}

			RF_ASSERT( Clock::kLowest < iter->second.mInitialConnectionTime );
			RF_ASSERT( iter->second.mInitialConnectionTime < now );
			iter->second.mLatestValidInboundData = now;

			validConnectionsChanged = true;
		}
	}

	// Need to send a connection list, an update to existing connections as
	//  well as an initial list for the newly trusted connections
	if( validConnectionsChanged )
	{
		RFLOG_INFO( nullptr, RFCAT_GAMESYNC, "Trust additions performed, need to send a connection update" );
		RF_TODO_BREAK();
	}
}

///////////////////////////////////////////////////////////////////////////////
}

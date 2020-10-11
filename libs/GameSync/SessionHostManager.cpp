#include "stdafx.h"
#include "SessionHostManager.h"

#include "Communication/EndpointManager.h"
#include "Logging/Logging.h"

#include "PlatformNetwork_win32/TCPSocket.h"

#include "core/ptr/default_creator.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

SessionHostManager::SessionHostManager( HostSpec spec )
	: mSpec( spec )
{
	//
}



bool SessionHostManager::IsHostingASession() const
{
	ReaderLock const listenThreadLock( mListenerThreadMutex );

	return mListenerThread.IsStarted();
}



void SessionHostManager::StartHostingASession()
{
	WriterLock const listenThreadLock( mListenerThreadMutex );

	if constexpr( config::kAsserts )
	{
		// Ensure there are no client connections
		{
			ReaderLock const connectionLock( mClientConnectionsMutex );

			RF_ASSERT( mClientConnections.empty() );
		}

		// Ensure there are no endpoints
		{
			ReaderLock const endpointLock( mEndpointManangerMutex );

			RF_TODO_BREAK();
		}
	}

	// Create listener socket
	{
		WriterLock const listenSocketLock( mListenerSocketMutex );

		RF_ASSERT( mListenerSocket != nullptr );
		mListenerSocket = DefaultCreator<platform::network::TCPSocket>::Create(
			platform::network::TCPSocket::BindServerSocket( mSpec.mIPv6, mSpec.mDevLoopback, mSpec.mPort ) );
		if( mListenerSocket->IsValid() == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESYNC,
				"Failed to bind a listener socket, hosted session will not be"
				" able to accept new clients. See logs for details." );
		}
	}

	RF_TODO_BREAK_MSG( "Initialize listener thread" );

	RF_ASSERT( mListenerThread.IsStarted() == false );
	mListenerThread.Start();
}



void SessionHostManager::StopHostingASession()
{
	WriterLock const listenThreadLock( mListenerThreadMutex );

	// Stop listener socket
	// NOTE: We have to do this, not the listener thread, as it is likely
	//  blocked waiting for a new connection
	{
		ReaderLock const listenSocketLock( mListenerSocketMutex );

		RF_ASSERT( mListenerSocket != nullptr );
		mListenerSocket->Shutdown();
	}

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
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "TCPSocket.h"

#include "PlatformNetwork_win32/AddressHelpers.h"

#include "Logging/Logging.h"

#include "core_platform/inc/winsock2_inc.h"

#include "core_math/math_casts.h"


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

TCPSocket::TCPSocket( TCPSocket&& rhs )
	: mShutdown( rhs.mShutdown.load( rftl::memory_order::memory_order_acquire ) )
	, RF_MOVE_CONSTRUCT( mListener )
{
	shim::SOCKET const transfer = rhs.mSocket.exchange( shim::kINVALID_SOCKET, rftl::memory_order::memory_order_acq_rel );
	mSocket.store( transfer, rftl::memory_order::memory_order_release );
}



TCPSocket& TCPSocket::operator=( TCPSocket&& rhs )
{
	if( this != &rhs )
	{
		CloseSocketIfValid();
		shim::SOCKET const transfer = rhs.mSocket.exchange( shim::kINVALID_SOCKET, rftl::memory_order::memory_order_acq_rel );
		mShutdown.store( rhs.mShutdown.load( rftl::memory_order::memory_order_acquire ), rftl::memory_order::memory_order_release );
		mListener = rhs.mListener;
		mSocket.store( transfer, rftl::memory_order::memory_order_release );
	}
	return *this;
}



TCPSocket::~TCPSocket()
{
	CloseSocketIfValid();
}



bool TCPSocket::IsValid() const
{
	return IsValidSocketHandle();
}



bool TCPSocket::IsListener() const
{
	return mListener;
}



TCPSocket TCPSocket::ConnectClientSocket( rftl::string hostname, uint16_t port )
{
	TCPSocket retVal = {};

	// There may be multiple ways to connect, ask OS for suggestions
	win32::addrinfo hints = {};
	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC; // Whatever works
	hints.ai_socktype = SOCK_STREAM; // Stream
	hints.ai_protocol = win32::IPPROTO_TCP; // TCP
	details::AddressList const suggestions = details::LookupSuggestedAddresses( hints, hostname.c_str(), port );

	// Try suggestions in order
	for( win32::addrinfo const* const& suggestionPtr : suggestions.mList )
	{
		RF_ASSERT( suggestionPtr != nullptr );
		win32::addrinfo const& suggestion = *suggestionPtr;

		rftl::string const name = details::GetAddress( *suggestion.ai_addr, suggestion.ai_addrlen );
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Attempting to create client TCP socket at \"%s\"", name.c_str() );

		// Create
		retVal.InitSocketHandle( win32::WSASocketW(
			suggestion.ai_family,
			suggestion.ai_socktype,
			suggestion.ai_protocol,
			nullptr, // No contraints
			0, // No grouping
			WSA_FLAG_OVERLAPPED ) ); // Allow async usage
		if( retVal.IsValidSocketHandle() == false )
		{
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create unconnected TCP socket: WSA %i", win32::WSAGetLastError() );
		}
		else
		{
			// Connect
			int const connectResult = win32::WSAConnect(
				retVal.GetMutableSocketHandle(),
				suggestion.ai_addr,
				math::integer_cast<int>( suggestion.ai_addrlen ),
				nullptr, // N/A on TCP
				nullptr, // N/A on TCP
				nullptr, // No flow-spec constraints
				nullptr ); // No flow-spec constraints
			if( connectResult != 0 )
			{
				RF_ASSERT( connectResult == SOCKET_ERROR );
				RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to connect TCP socket: WSA %i", win32::WSAGetLastError() );
			}
			else
			{
				// Success!
				RF_ASSERT( retVal.IsValid() );
				RFLOG_MILESTONE( nullptr, RFCAT_PLATFORMNETWORK, "Connected a TCP client socket at \"%s\"", name.c_str() );
				return retVal;
			}
		}

		// Reset and try next suggestion
		retVal = {};
	}

	RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create and connect TCP socket after trying %llu interfaces", suggestions.mList.size() );
	return retVal;
}



TCPSocket TCPSocket::BindServerSocket( bool preferIPv6, bool loopback, uint16_t port )
{
	TCPSocket retVal = {};

	// There may be multiple interfaces, ask OS for suggestions
	win32::addrinfo hints = {};
	hints.ai_flags = AI_PASSIVE; // Expect bind(...)
	hints.ai_family = preferIPv6 ? AF_INET6 : AF_INET; // IPv4 / IPv6
	hints.ai_socktype = SOCK_STREAM; // Stream
	hints.ai_protocol = win32::IPPROTO_TCP; // TCP
	char const* const serverHostname = loopback ? "localhost" : nullptr;
	details::AddressList const suggestions = details::LookupSuggestedAddresses( hints, serverHostname, port );

	// Try suggestions in order
	for( win32::addrinfo const* const& suggestionPtr : suggestions.mList )
	{
		RF_ASSERT( suggestionPtr != nullptr );
		win32::addrinfo const& suggestion = *suggestionPtr;

		rftl::string const name = details::GetAddress( *suggestion.ai_addr, suggestion.ai_addrlen );
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Attempting to create server TCP socket at \"%s\"", name.c_str() );

		// Create
		retVal.InitSocketHandle( win32::WSASocketW(
			suggestion.ai_family,
			suggestion.ai_socktype,
			suggestion.ai_protocol,
			nullptr, // No contraints
			0, // No grouping
			WSA_FLAG_OVERLAPPED ) ); // Allow async usage
		if( retVal.IsValidSocketHandle() == false )
		{
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create unbound TCP socket: WSA %i", win32::WSAGetLastError() );
		}
		else
		{
			// Bind
			int const bindResult = win32::bind(
				retVal.GetMutableSocketHandle(),
				suggestion.ai_addr,
				math::integer_cast<int>( suggestion.ai_addrlen ) );
			if( bindResult != 0 )
			{
				RF_ASSERT( bindResult == SOCKET_ERROR );
				RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to bind TCP socket: WSA %i", win32::WSAGetLastError() );
			}
			else
			{
				// Listen
				int const listenResult = win32::listen(
					retVal.GetMutableSocketHandle(),
					SOMAXCONN );
				if( listenResult != 0 )
				{
					RF_ASSERT( listenResult == SOCKET_ERROR );
					RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to listen on TCP socket: WSA %i", win32::WSAGetLastError() );
				}
				else
				{
					// Success!
					RF_ASSERT( retVal.IsValid() );
					retVal.mListener = true;
					RFLOG_MILESTONE( nullptr, RFCAT_PLATFORMNETWORK, "Bound a listening TCP server socket at \"%s\"", name.c_str() );
					return retVal;
				}
			}
		}

		// Reset and try next suggestion
		retVal = {};
	}

	RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create and bind TCP socket: WSA %i", win32::WSAGetLastError() );
	return retVal;
}



TCPSocket TCPSocket::WaitForNewClientConnection( TCPSocket& listeningServerSocket )
{
	TCPSocket retVal = {};

	RF_ASSERT( listeningServerSocket.IsListener() );

	// Accept
	win32::sockaddr_storage newAddress = {};
	win32::INT newAddressLen = sizeof( newAddress );
	retVal.InitSocketHandle( win32::WSAAccept(
		listeningServerSocket.GetMutableSocketHandle(),
		reinterpret_cast<win32::sockaddr*>( &newAddress ),
		&newAddressLen,
		nullptr, // No conditional check
		0 ) ); // No conditional check
	if( retVal.IsValidSocketHandle() == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to accept a TCP socket: WSA %i", win32::WSAGetLastError() );
	}
	else
	{
		rftl::string const name = details::GetAddress( *reinterpret_cast<win32::sockaddr*>( &newAddress ), math::integer_cast<size_t>( newAddressLen ) );
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Accepted a TCP connection from \"%s\"", name.c_str() );
	}

	return retVal;
}



bool TCPSocket::PeekAcceptNewClientNonBlocking()
{
	RF_ASSERT( IsListener() );
	return PeekReadNonBlocking();
}



bool TCPSocket::PeekSendNonBlocking()
{
	RF_ASSERT( IsListener() == false );
	return PeekWriteNonBlocking();
}



bool TCPSocket::PeekReceiveNonBlocking()
{
	RF_ASSERT( IsListener() == false );
	return PeekReadNonBlocking();
}



bool TCPSocket::SendBuffer( Buffer const& buffer )
{
	RF_ASSERT( buffer.empty() == false );

	// HACK: Treating buffer as non-const, assumed to be yet another design
	//  flaw in Microsoft APIs and that no modifications will actually happen
	Buffer& HACK_nonConstBuffer = *const_cast<Buffer*>( &buffer );

	rftl::array<win32::WSABUF, 1> wsaBufs = {};
	wsaBufs.at( 0 ).buf = reinterpret_cast<win32::CHAR*>( HACK_nonConstBuffer.data() );
	wsaBufs.at( 0 ).len = math::integer_cast<win32::ULONG>( buffer.size() );

	// Send
	win32::DWORD bytesSent = 0;
	int const sendResult = win32::WSASend(
		GetMutableSocketHandle(),
		wsaBufs.data(),
		math::integer_cast<win32::DWORD>( wsaBufs.size() ),
		&bytesSent,
		0, // No flags
		nullptr, // No overlapped
		nullptr ); // No overlapped
	if( sendResult != 0 )
	{
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to send TCP socket data: WSA %i", win32::WSAGetLastError() );
		return false;
	}
	else if( bytesSent != buffer.size() )
	{
		// This should never occur on a blocking call
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to send some TCP socket data" );
		return false;
	}

	RFLOG_TRACE( nullptr, RFCAT_PLATFORMNETWORK, "Sent TCP data (%lu bytes)", bytesSent );
	return true;
}



bool TCPSocket::ReceiveBuffer( Buffer& buffer, size_t maxLen )
{
	buffer.resize( maxLen );

	rftl::array<win32::WSABUF, 1> wsaBufs = {};
	wsaBufs.at( 0 ).buf = reinterpret_cast<win32::CHAR*>( buffer.data() );
	wsaBufs.at( 0 ).len = math::integer_cast<win32::ULONG>( buffer.size() );

	// Receive
	win32::DWORD flags = MSG_PUSH_IMMEDIATE; // Get data ASAP
	win32::DWORD bytesReceived = 0;
	int const receiveResult = win32::WSARecv(
		GetMutableSocketHandle(),
		wsaBufs.data(),
		math::integer_cast<win32::DWORD>( wsaBufs.size() ),
		&bytesReceived,
		&flags,
		nullptr, // No overlapped
		nullptr ); // No overlapped
	if( receiveResult != 0 )
	{
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to receive TCP socket data: WSA %i", win32::WSAGetLastError() );
		buffer.clear();
		return false;
	}
	else if( bytesReceived == 0 )
	{
		// This should indicate a graceful connection close
		RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to receive any TCP socket data" );
		buffer.clear();
		return false;
	}

	RFLOG_TRACE( nullptr, RFCAT_PLATFORMNETWORK, "Received TCP data (%lu bytes)", bytesReceived );
	RF_ASSERT( bytesReceived <= buffer.size() );
	buffer.resize( bytesReceived );
	return true;
}



void TCPSocket::Shutdown()
{
	RF_TODO_ANNOTATION( "Implement a cleaner/safer way to do this" );
	if constexpr( false )
	{
		// See internal comments, this doesn't work
		ShutdownSocketIfValid();
	}
	else
	{
		// This is theoretically not thread-safe, because a socket identifier
		//  can be re-used once closed. However, since shutdown doesn't work,
		//  this is the only viable option for stopping blocking calls without
		//  a much more elaborate setup. The expectation is that identifier
		//  re-use will happen across a large enough time-scale that it will
		//  note result in bugs under normal circumstances.
		// TODO: Evaluate a much more convoluted setup that uses
		//  WSAEventSelect(...) and WaitForMultipleObjectsEx(...) for some
		//  overly contrived signalling+blocking system to abort blocking
		//  socket calls on shutdown.
		CloseSocketIfValid();
	}
}

///////////////////////////////////////////////////////////////////////////////

bool TCPSocket::MakeNonBlocking()
{
	win32::u_long one = 1;
	static constexpr uint32_t kFIONBIO = _IOW( 'f', 126, win32::u_long );
	int const nonblockingResult = win32::ioctlsocket( GetMutableSocketHandle(), static_cast<long>( kFIONBIO ), &one );
	if( nonblockingResult != 0 )
	{
		RF_ASSERT( nonblockingResult == SOCKET_ERROR );
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to make TCP socket non-blocking: WSA %i", win32::WSAGetLastError() );
		return false;
	}
	return true;
}



bool TCPSocket::IsValidSocketHandle() const
{
	return mSocket.load( rftl::memory_order::memory_order_acquire ) != shim::kINVALID_SOCKET;
}



shim::SOCKET TCPSocket::GetMutableSocketHandle()
{
	return mSocket.load( rftl::memory_order::memory_order_acquire );
}



void TCPSocket::InitSocketHandle( shim::SOCKET socket )
{
	RF_ASSERT( IsValid() == false );
	mSocket.store( socket, rftl::memory_order::memory_order_release );
}



void TCPSocket::ClearSocketHandle()
{
	mSocket.store( shim::kINVALID_SOCKET, rftl::memory_order::memory_order_release );
}



bool TCPSocket::PeekWriteNonBlocking()
{
	win32::fd_set socketSet = {};
	socketSet.fd_count = 1;
	socketSet.fd_array[0] = mSocket.load( rftl::memory_order::memory_order_acquire );

	win32::timeval const timeout = { 0, 0 };
	int const selectResult = win32::select(
		0, // Unused legacy param
		nullptr, // Read
		&socketSet, // Write
		nullptr, // Error state
		&timeout );

	return selectResult == 1;
}



bool TCPSocket::PeekReadNonBlocking()
{
	win32::fd_set socketSet = {};
	socketSet.fd_count = 1;
	socketSet.fd_array[0] = mSocket.load( rftl::memory_order::memory_order_acquire );

	win32::timeval const timeout = { 0, 0 };
	int const selectResult = win32::select(
		0, // Unused legacy param
		&socketSet, // Read
		nullptr, // Write
		nullptr, // Error state
		&timeout );

	return selectResult == 1;
}



void TCPSocket::ShutdownSocketIfValid()
{
	if( IsValid() == false )
	{
		return;
	}

	bool const previousShutdown = mShutdown.exchange( true, rftl::memory_order::memory_order_acq_rel );
	if( previousShutdown )
	{
		// Already shutdown
		return;
	}

	// Make non-blocking
	// NOTE: This does not work, accept(...) calls are not interrupted
	MakeNonBlocking();

	if( mListener )
	{
		// Listeners cannot be 'shutdown' because they are not yet connected
		// TODO: An equivalent concept for disabling listeners that will stop
		//  any new accept(...) calls, and interrupting any that are blocking
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Shutdown a listening TCP socket" );
	}
	else
	{
		// Disable further reads/writes
		int const shutdownResult = win32::shutdown( GetMutableSocketHandle(), SD_BOTH );
		if( shutdownResult != 0 )
		{
			RF_ASSERT( shutdownResult == SOCKET_ERROR );
			int const lastError = win32::WSAGetLastError();
			if( lastError == WSAENOTCONN )
			{
				RFLOG_TRACE( nullptr, RFCAT_PLATFORMNETWORK, "Failed to shutdown TCP socket because it was already disconnected" );
			}
			else
			{
				RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to shutdown TCP socket: WSA %i", lastError );
			}
		}
		else
		{
			RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Shutdown a connected TCP socket" );
		}
	}
}



void TCPSocket::CloseSocketIfValid()
{
	if( IsValid() == false )
	{
		return;
	}

	ShutdownSocketIfValid();

	int const result = win32::closesocket( GetMutableSocketHandle() );
	if( result != 0 )
	{
		RF_ASSERT( result == SOCKET_ERROR );
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to close TCP socket: WSA %i", win32::WSAGetLastError() );
	}
	ClearSocketHandle();
}

///////////////////////////////////////////////////////////////////////////////
}

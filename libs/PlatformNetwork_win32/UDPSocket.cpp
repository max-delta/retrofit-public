#include "stdafx.h"
#include "UDPSocket.h"

#include "PlatformNetwork_win32/AddressHelpers.h"

#include "Logging/Logging.h"

#include "core_platform/inc/winsock2_inc.h"

#include "core_math/math_casts.h"


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

UDPSocket::UDPSocket( UDPSocket&& rhs )
	: mShutdown( rhs.mShutdown.load( rftl::memory_order::memory_order_acquire ) )
{
	shim::SOCKET const transfer = rhs.mSocket.exchange( shim::kINVALID_SOCKET, rftl::memory_order::memory_order_acq_rel );
	mSocket.store( transfer, rftl::memory_order::memory_order_release );
}



UDPSocket& UDPSocket::operator=( UDPSocket&& rhs )
{
	if( this != &rhs )
	{
		CloseSocketIfValid();
		shim::SOCKET const transfer = rhs.mSocket.exchange( shim::kINVALID_SOCKET, rftl::memory_order::memory_order_acq_rel );
		mShutdown.store( rhs.mShutdown.load( rftl::memory_order::memory_order_acquire ), rftl::memory_order::memory_order_release );
		mSocket.store( transfer, rftl::memory_order::memory_order_release );
	}
	return *this;
}



UDPSocket::~UDPSocket()
{
	CloseSocketIfValid();
}



bool UDPSocket::IsValid() const
{
	return IsValidSocketHandle();
}



UDPSocket UDPSocket::BindSocket( bool preferIPv6, bool loopback, uint16_t port )
{
	UDPSocket retVal = {};

	// There may be multiple interfaces, ask OS for suggestions
	win32::addrinfo hints = {};
	hints.ai_flags = AI_PASSIVE; // Expect bind(...)
	hints.ai_family = preferIPv6 ? AF_INET6 : AF_INET; // IPv4 / IPv6
	hints.ai_socktype = SOCK_DGRAM; // Datagram
	hints.ai_protocol = win32::IPPROTO_UDP; // UDP
	char const* const serverHostname = loopback ? "localhost" : nullptr;
	details::AddressList const suggestions = details::LookupSuggestedAddresses( hints, serverHostname, port );

	// Try suggestions in order
	for( win32::addrinfo const* const& suggestionPtr : suggestions.mList )
	{
		RF_ASSERT( suggestionPtr != nullptr );
		win32::addrinfo const& suggestion = *suggestionPtr;

		rftl::string const name = details::GetAddress( *suggestion.ai_addr, suggestion.ai_addrlen );
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Attempting to create server UDP socket at \"%s\"", name.c_str() );

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
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create unbound UDP socket: WSA %i", win32::WSAGetLastError() );
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
				RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to bind UDP socket: WSA %i", win32::WSAGetLastError() );
			}
			else
			{
				// Success!
				RF_ASSERT( retVal.IsValid() );
				RFLOG_MILESTONE( nullptr, RFCAT_PLATFORMNETWORK, "Bound a UDP socket at \"%s\"", name.c_str() );
				return retVal;
			}
		}

		// Reset and try next suggestion
		retVal = {};
	}

	RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create and bind UDP socket: WSA %i", win32::WSAGetLastError() );
	return retVal;
}



bool UDPSocket::PeekSendNonBlocking()
{
	return PeekWriteNonBlocking();
}



bool UDPSocket::PeekReceiveNonBlocking()
{
	return PeekReadNonBlocking();
}



bool UDPSocket::SendBuffer( Buffer const& buffer, IPv4Address const& address, uint16_t port )
{
	win32::sockaddr_storage destinationStorage = {};
	win32::sockaddr const& destination = *reinterpret_cast<win32::sockaddr*>( &destinationStorage );
	size_t destinationLen = 0;
	bool const addrResult = details::BuildIPv4Address( destinationStorage, destinationLen, address, port );
	if( addrResult == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to build IPv4 address" );
		return false;
	}

	return SendBuffer( buffer, destination, destinationLen, port );
}



bool UDPSocket::SendBuffer( Buffer const& buffer, IPv6Address const& address, uint16_t port )
{
	win32::sockaddr_storage destinationStorage = {};
	win32::sockaddr const& destination = *reinterpret_cast<win32::sockaddr*>( &destinationStorage );
	size_t destinationLen = 0;
	bool const addrResult = details::BuildIPv6Address( destinationStorage, destinationLen, address, port );
	if( addrResult == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to build IPv6 address" );
		return false;
	}

	return SendBuffer( buffer, destination, destinationLen, port );
}



bool UDPSocket::ReceiveBuffer( Buffer& buffer, size_t maxLen )
{
	buffer.resize( maxLen );

	rftl::array<win32::WSABUF, 1> wsaBufs = {};
	wsaBufs.at( 0 ).buf = reinterpret_cast<win32::CHAR*>( buffer.data() );
	wsaBufs.at( 0 ).len = math::integer_cast<win32::ULONG>( buffer.size() );

	// Receive
	win32::DWORD flags = MSG_PUSH_IMMEDIATE; // Get data ASAP
	win32::DWORD bytesReceived = 0;
	int const receiveResult = win32::WSARecvFrom(
		GetMutableSocketHandle(),
		wsaBufs.data(),
		math::integer_cast<win32::DWORD>( wsaBufs.size() ),
		&bytesReceived,
		&flags,
		nullptr, // Ignore source
		nullptr, // Ignore source
		nullptr, // No overlapped
		nullptr ); // No overlapped
	if( receiveResult != 0 )
	{
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to receive UDP socket data: WSA %i", win32::WSAGetLastError() );
		buffer.clear();
		return false;
	}
	else if( bytesReceived == 0 )
	{
		// This should indicate a graceful connection close
		RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to receive any UDP socket data" );
		buffer.clear();
		return false;
	}

	RFLOG_TRACE( nullptr, RFCAT_PLATFORMNETWORK, "Received UDP data (%lu bytes)", bytesReceived );
	RF_ASSERT( bytesReceived <= buffer.size() );
	buffer.resize( bytesReceived );
	return true;
}



void UDPSocket::Shutdown()
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

bool UDPSocket::MakeNonBlocking()
{
	win32::u_long one = 1;
	static constexpr uint32_t kFIONBIO = _IOW( 'f', 126, win32::u_long );
	int const nonblockingResult = win32::ioctlsocket( GetMutableSocketHandle(), static_cast<long>( kFIONBIO ), &one );
	if( nonblockingResult != 0 )
	{
		RF_ASSERT( nonblockingResult == SOCKET_ERROR );
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to make UDP socket non-blocking: WSA %i", win32::WSAGetLastError() );
		return false;
	}
	return true;
}



bool UDPSocket::IsValidSocketHandle() const
{
	return mSocket.load( rftl::memory_order::memory_order_acquire ) != shim::kINVALID_SOCKET;
}



shim::SOCKET UDPSocket::GetMutableSocketHandle()
{
	return mSocket.load( rftl::memory_order::memory_order_acquire );
}



void UDPSocket::InitSocketHandle( shim::SOCKET socket )
{
	RF_ASSERT( IsValid() == false );
	mSocket.store( socket, rftl::memory_order::memory_order_release );
}



void UDPSocket::ClearSocketHandle()
{
	mSocket.store( shim::kINVALID_SOCKET, rftl::memory_order::memory_order_release );
}



bool UDPSocket::PeekWriteNonBlocking()
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



bool UDPSocket::PeekReadNonBlocking()
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



bool UDPSocket::SendBuffer( Buffer const& buffer, win32::sockaddr const& destination, size_t destinationLen, uint16_t port )
{
	// HACK: Treating buffer as non-const, assumed to be yet another design
	//  flaw in Microsoft APIs and that no modifications will actually happen
	Buffer& HACK_nonConstBuffer = *const_cast<Buffer*>( &buffer );

	rftl::array<win32::WSABUF, 1> wsaBufs = {};
	wsaBufs.at( 0 ).buf = reinterpret_cast<win32::CHAR*>( HACK_nonConstBuffer.data() );
	wsaBufs.at( 0 ).len = math::integer_cast<win32::ULONG>( buffer.size() );

	// Send
	win32::DWORD bytesSent = 0;
	int const sendResult = win32::WSASendTo(
		GetMutableSocketHandle(),
		wsaBufs.data(),
		math::integer_cast<win32::DWORD>( wsaBufs.size() ),
		&bytesSent,
		0, // No flags
		&destination,
		math::integer_cast<int>( destinationLen ),
		nullptr, // No overlapped
		nullptr ); // No overlapped
	if( sendResult != 0 )
	{
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to send UDP socket data: WSA %i", win32::WSAGetLastError() );
		return false;
	}
	else if( bytesSent != buffer.size() )
	{
		// This should never occur on a blocking call
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to send some UDP socket data" );
		return false;
	}

	RFLOG_TRACE( nullptr, RFCAT_PLATFORMNETWORK, "Sent UDP data (%lu bytes)", bytesSent );
	return true;
}



void UDPSocket::ShutdownSocketIfValid()
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
	MakeNonBlocking();

	// Disable further reads/writes
	int const shutdownResult = win32::shutdown( GetMutableSocketHandle(), SD_BOTH );
	if( shutdownResult != 0 )
	{
		RF_ASSERT( shutdownResult == SOCKET_ERROR );
		int const lastError = win32::WSAGetLastError();
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to shutdown UDP socket: WSA %i", lastError );
	}
	else
	{
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Shutdown a UDP socket" );
	}
}



void UDPSocket::CloseSocketIfValid()
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
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to close UDP socket: WSA %i", win32::WSAGetLastError() );
	}
	ClearSocketHandle();
}

///////////////////////////////////////////////////////////////////////////////
}

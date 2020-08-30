#include "stdafx.h"
#include "TCPSocket.h"

#include "Logging/Logging.h"

#include "core_platform/winsock2_inc.h"

#include "core_math/math_casts.h"

#include "rftl/vector"


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// Addresses have pointer graphs that need free-ing
struct AddressList
{
	~AddressList()
	{
		if( mHead != nullptr )
		{
			win32::freeaddrinfo( mHead );
			mHead = nullptr;
		}
	}

public:
	rftl::vector<win32::addrinfo> mList;

private:
	friend AddressList LookupSuggestedAddresses( win32::addrinfo const& hints, char const* hostnameHint, uint16_t portHint );
	win32::addrinfo* mHead = nullptr;
};



void SortSuggestions( AddressList& suggestions )
{
	// No known reason to sort currently, using OS order
}



AddressList LookupSuggestedAddresses( win32::addrinfo const& hints, char const* hostnameHint, uint16_t portHint )
{
	AddressList retVal = {};

	// Lookup
	rftl::string const portHintAsString = rftl::to_string( portHint );
	int const suggestionResult = win32::getaddrinfo(
		hostnameHint,
		portHintAsString.c_str(),
		&hints,
		&retVal.mHead );
	if( suggestionResult != 0 )
	{
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to get suggested addresses: ret %i", suggestionResult );
		return {};
	}

	// Store
	win32::addrinfo const* iter = retVal.mHead;
	while( iter != nullptr )
	{
		win32::addrinfo& back = retVal.mList.emplace_back( *iter );
		back.ai_next = nullptr;
		iter = iter->ai_next;
	}

	// Sort
	SortSuggestions( retVal );

	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

TCPSocket::TCPSocket( TCPSocket&& rhs )
	: RF_MOVE_CONSTRUCT( mSocket )
{
	rhs.mSocket = shim::kINVALID_SOCKET;
}



TCPSocket& TCPSocket::operator=( TCPSocket&& rhs )
{
	if( this != &rhs )
	{
		CloseSocketIfValid();
		RF_MOVE_ASSIGN( mSocket );
		rhs.InvalidateSocket();
	}
	return *this;
}



TCPSocket::~TCPSocket()
{
	CloseSocketIfValid();
}



bool TCPSocket::IsValid() const
{
	return mSocket != shim::kINVALID_SOCKET;
}



shim::SOCKET TCPSocket::GetSocketHandle()
{
	return mSocket;
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
	for( win32::addrinfo const& suggestion : suggestions.mList )
	{
		// Create
		retVal.mSocket = win32::socket(
			suggestion.ai_family,
			suggestion.ai_socktype,
			suggestion.ai_protocol );
		if( retVal.mSocket == shim::kINVALID_SOCKET )
		{
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create unconnected TCP socket: WSA %i", win32::WSAGetLastError() );
		}
		else
		{
			// Bind
			int const connectResult = win32::connect(
				retVal.mSocket,
				suggestion.ai_addr,
				math::integer_cast<int>( suggestion.ai_addrlen ) );
			if( connectResult != 0 )
			{
				RF_ASSERT( connectResult == SOCKET_ERROR );
				RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to connect TCP socket: WSA %i", win32::WSAGetLastError() );
			}
			else
			{
				// Success!
				RF_ASSERT( retVal.IsValid() );
				RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Connected a TCP client socket for port %u", port );
				return retVal;
			}
		}

		// Reset and try next suggestion
		retVal = {};
	}

	RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create and connect TCP socket: WSA %i", win32::WSAGetLastError() );
	return retVal;
}



TCPSocket TCPSocket::BindServerSocket( bool ipv6, uint16_t port )
{
	TCPSocket retVal = {};

	// There may be multiple interfaces, ask OS for suggestions
	win32::addrinfo hints = {};
	hints.ai_flags = AI_PASSIVE; // Expect bind(...)
	hints.ai_family = ipv6 ? AF_INET6 : AF_INET; // IPv4 / IPv6
	hints.ai_socktype = SOCK_STREAM; // Stream
	hints.ai_protocol = win32::IPPROTO_TCP; // TCP
	static constexpr char const* kServerHostname = nullptr; // TODO: Support?
	details::AddressList const suggestions = details::LookupSuggestedAddresses( hints, kServerHostname, port );

	// Try suggestions in order
	for( win32::addrinfo const& suggestion : suggestions.mList )
	{
		// Create
		retVal.mSocket = win32::socket(
			suggestion.ai_family,
			suggestion.ai_socktype,
			suggestion.ai_protocol );
		if( retVal.mSocket == shim::kINVALID_SOCKET )
		{
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create unbound TCP socket: WSA %i", win32::WSAGetLastError() );
		}
		else
		{
			// Bind
			int const bindResult = win32::bind(
				retVal.mSocket,
				suggestion.ai_addr,
				math::integer_cast<int>( suggestion.ai_addrlen ) );
			if( bindResult != 0 )
			{
				RF_ASSERT( bindResult == SOCKET_ERROR );
				RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to bind TCP socket: WSA %i", win32::WSAGetLastError() );
			}
			else
			{
				// Success!
				RF_ASSERT( retVal.IsValid() );
				RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Bound a TCP server socket for port %u", port );
				return retVal;
			}
		}

		// Reset and try next suggestion
		retVal = {};
	}

	RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create and bind TCP socket: WSA %i", win32::WSAGetLastError() );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

void TCPSocket::CloseSocketIfValid()
{
	if( mSocket != shim::kINVALID_SOCKET )
	{
		int const result = win32::closesocket( mSocket );
		if( result != 0 )
		{
			RF_ASSERT( result == SOCKET_ERROR );
			RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to close TCP socket: WSA %i", win32::WSAGetLastError() );
		}
		mSocket = shim::kINVALID_SOCKET;
	}
}



void TCPSocket::InvalidateSocket()
{
	mSocket = shim::kINVALID_SOCKET;
}

///////////////////////////////////////////////////////////////////////////////
}

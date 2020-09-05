#include "stdafx.h"
#include "TCPSocket.h"

#include "Logging/Logging.h"

#include "core_platform/winsock2_inc.h"

#include "core_math/math_casts.h"
#include "core_math/math_bytes.h"

#include "rftl/vector"


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// Addresses have pointer graphs that need free-ing
struct AddressList
{
	RF_NO_COPY( AddressList );

public:
	AddressList() = default;
	AddressList( AddressList&& rhs )
		: RF_MOVE_CONSTRUCT( mList )
		, RF_MOVE_CONSTRUCT( mHead )
	{
		rhs.mHead = nullptr;
	}

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
		RF_ASSERT( back.ai_family == AF_INET || back.ai_family == AF_INET6 );
		back.ai_next = nullptr;
		iter = iter->ai_next;
	}

	// Sort
	SortSuggestions( retVal );

	return retVal;
}



rftl::string GetAddress( win32::sockaddr const& sockAddr, size_t sockAddrLen )
{
	if( sockAddr.sa_family == AF_INET )
	{
		win32::sockaddr_in const& ipv4 = *reinterpret_cast<win32::sockaddr_in const*>( &sockAddr );
		uint16_t const port = win32::ntohs( ipv4.sin_port );
		auto const& bytes = ipv4.sin_addr.S_un.S_un_b;
		static constexpr char kExample[] = "255.255.255.255:65535";
		rftl::array<char, sizeof( kExample )> buf;
		snprintf( buf.data(), buf.size(), "%u.%u.%u.%u:%u", bytes.s_b1, bytes.s_b2, bytes.s_b3, bytes.s_b4, port );
		buf.back() = '\0';
		return buf.data();
	}
	else if( sockAddr.sa_family == AF_INET6 )
	{
		win32::sockaddr_in6 const& ipv6 = *reinterpret_cast<win32::sockaddr_in6 const*>( &sockAddr );
		uint16_t const port = win32::ntohs( ipv6.sin6_port );
		uint8_t const( &bytes )[16] = ipv6.sin6_addr.u.Byte;
		static constexpr char kExample[] = "[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535";
		rftl::array<uint16_t, 8> hextets = {};
		for( size_t i = 0; i < 16; i += 2 )
		{
			hextets.at( i / 2 ) = math::integer_cast<uint16_t>( ( bytes[i] << 8 ) | bytes[i + 1] );
			if constexpr( compiler::kEndianness == compiler::Endianness::Big )
			{
				hextets.at( i / 2 ) = math::ReverseByteOrder( hextets.at( i / 2 ) );
			}
		}
		rftl::array<char, sizeof( kExample )> buf;
		snprintf( buf.data(), buf.size(), "[%x:%x:%x:%x:%x:%x:%x:%x]:%u",
			hextets[0], hextets[1], hextets[2], hextets[3],
			hextets[4], hextets[5], hextets[6], hextets[7], port );
		buf.back() = '\0';
		return buf.data();
	}
	else
	{
		return "<UNKNOWN>";
	}
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



bool TCPSocket::IsServer() const
{
	return mServer;
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
		rftl::string const name = details::GetAddress( *suggestion.ai_addr, suggestion.ai_addrlen );
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Attempting to create client TCP socket at \"%s\"", name.c_str() );

		// Create
		retVal.mSocket = win32::WSASocketW(
			suggestion.ai_family,
			suggestion.ai_socktype,
			suggestion.ai_protocol,
			nullptr, // No contraints
			0, // No grouping
			WSA_FLAG_OVERLAPPED ); // Allow async usage
		if( retVal.mSocket == shim::kINVALID_SOCKET )
		{
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create unconnected TCP socket: WSA %i", win32::WSAGetLastError() );
		}
		else
		{
			// Connect
			int const connectResult = win32::WSAConnect(
				retVal.mSocket,
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

	RFLOG_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to create and connect TCP socket: WSA %i", win32::WSAGetLastError() );
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
	for( win32::addrinfo const& suggestion : suggestions.mList )
	{
		rftl::string const name = details::GetAddress( *suggestion.ai_addr, suggestion.ai_addrlen );
		RFLOG_INFO( nullptr, RFCAT_PLATFORMNETWORK, "Attempting to create server TCP socket at \"%s\"", name.c_str() );

		// Create
		retVal.mSocket = win32::WSASocketW(
			suggestion.ai_family,
			suggestion.ai_socktype,
			suggestion.ai_protocol,
			nullptr, // No contraints
			0, // No grouping
			WSA_FLAG_OVERLAPPED ); // Allow async usage
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
				// Listen
				int const listenResult = win32::listen(
					retVal.mSocket,
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
					retVal.mServer = true;
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

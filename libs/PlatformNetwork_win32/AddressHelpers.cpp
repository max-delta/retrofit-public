#include "stdafx.h"
#include "AddressHelpers.h"

#include "Logging/Logging.h"

#include "core_platform/inc/winsock2_inc.h"

#include "core_math/math_casts.h"
#include "core_math/math_bytes.h"


namespace RF::platform::network::details {
///////////////////////////////////////////////////////////////////////////////

AddressList::AddressList( AddressList&& rhs )
	: RF_MOVE_CONSTRUCT( mList )
	, RF_MOVE_CONSTRUCT( mHead )
{
	rhs.mHead = nullptr;
}



AddressList::~AddressList()
{
	if( mHead != nullptr )
	{
		win32::freeaddrinfo( mHead );
		mHead = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////

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
		RFLOGF_ERROR( nullptr, RFCAT_PLATFORMNETWORK, "Failed to get suggested addresses: ret {}", suggestionResult );
		return {};
	}

	// Store
	win32::addrinfo const* iter = retVal.mHead;
	while( iter != nullptr )
	{
		RF_ASSERT( iter->ai_family == AF_INET || iter->ai_family == AF_INET6 );
		retVal.mList.emplace_back( iter );
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



bool BuildIPv4Address( win32::sockaddr_storage& sockAddr, size_t& sockAddrLen, IPv4Address const& address, uint16_t port )
{
	sockAddr = {};
	sockAddrLen = 0;

	win32::sockaddr_in& ipv4 = *reinterpret_cast<win32::sockaddr_in*>( &sockAddr );
	ipv4 = {};
	ipv4.sin_family = AF_INET;
	ipv4.sin_port = win32::htons( port );
	static_assert( IPv4Address().size() == 4 );
	ipv4.sin_addr.S_un.S_un_b.s_b1 = address.at( 0 );
	ipv4.sin_addr.S_un.S_un_b.s_b2 = address.at( 1 );
	ipv4.sin_addr.S_un.S_un_b.s_b3 = address.at( 2 );
	ipv4.sin_addr.S_un.S_un_b.s_b4 = address.at( 3 );
	sockAddrLen = sizeof( win32::sockaddr_in );

	return true;
}



bool BuildIPv6Address( win32::sockaddr_storage& sockAddr, size_t& sockAddrLen, IPv6Address const& address, uint16_t port )
{
	sockAddr = {};
	sockAddrLen = 0;

	win32::sockaddr_in6& ipv6 = *reinterpret_cast<win32::sockaddr_in6*>( &sockAddr );
	ipv6 = {};
	ipv6.sin6_family = AF_INET6;
	ipv6.sin6_port = win32::htons( port );
	static_assert( rftl::extent<decltype( ipv6.sin6_addr.u.Byte )>::value == 16 );
	static_assert( IPv6Address().size() == 16 );
	for( size_t i = 0; i < 16; i++ )
	{
		ipv6.sin6_addr.u.Byte[i] = address.at( i );
	}
	sockAddrLen = sizeof( win32::sockaddr_in6 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

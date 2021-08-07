#pragma once
#include "project.h"

#include "core_platform/shim/win_shim.h"
#include "core_platform/IPAddressFwd.h"

#include "rftl/string"
#include "rftl/vector"


// Forwards
namespace win32 {
struct addrinfo;
struct sockaddr;
struct sockaddr_storage;
}

namespace RF::platform::network::details {
///////////////////////////////////////////////////////////////////////////////

// Addresses have pointer graphs that need free-ing
struct AddressList
{
	RF_NO_COPY( AddressList );

public:
	AddressList() = default;
	AddressList( AddressList&& rhs );
	~AddressList();

public:
	rftl::vector<win32::addrinfo const*> mList;

private:
	friend AddressList LookupSuggestedAddresses( win32::addrinfo const& hints, char const* hostnameHint, uint16_t portHint );
	win32::addrinfo* mHead = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

void SortSuggestions( AddressList& suggestions );
AddressList LookupSuggestedAddresses( win32::addrinfo const& hints, char const* hostnameHint, uint16_t portHint );
rftl::string GetAddress( win32::sockaddr const& sockAddr, size_t sockAddrLen );

///////////////////////////////////////////////////////////////////////////////

bool BuildIPv4Address( win32::sockaddr_storage& sockAddr, size_t& sockAddrLen, IPv4Address const& address, uint16_t port );
bool BuildIPv6Address( win32::sockaddr_storage& sockAddr, size_t& sockAddrLen, IPv6Address const& address, uint16_t port );

///////////////////////////////////////////////////////////////////////////////
}

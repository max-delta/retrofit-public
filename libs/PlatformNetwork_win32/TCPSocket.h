#pragma once
#include "project.h"

#include "core_platform/win_shim.h"

#include "rftl/string"


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMNETWORK_API TCPSocket final
{
	RF_NO_COPY( TCPSocket );

	//
	// Public methods
public:
	TCPSocket() = default;
	TCPSocket( TCPSocket&& rhs );
	TCPSocket& operator=( TCPSocket&& rhs );
	~TCPSocket();

	bool IsValid() const;
	bool IsServer() const;
	shim::SOCKET GetSocketHandle();

	// NOTE: An empty hostname will result in attempting connections to local
	//  addresses, as opposed to "localhost" for loopback addresses
	static TCPSocket ConnectClientSocket( rftl::string hostname, uint16_t port );
	static TCPSocket BindServerSocket( bool preferIPv6, bool loopback, uint16_t port );


	//
	// Private methods
private:
	void CloseSocketIfValid();
	void InvalidateSocket();


	//
	// Private data
private:
	bool mServer = false;
	shim::SOCKET mSocket = shim::kINVALID_SOCKET;
};

///////////////////////////////////////////////////////////////////////////////
}

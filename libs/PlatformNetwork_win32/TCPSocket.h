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
	shim::SOCKET GetSocketHandle();

	static TCPSocket ConnectClientSocket( rftl::string hostname, uint16_t port );
	static TCPSocket BindServerSocket( bool ipv6, uint16_t port );


	//
	// Private methods
private:
	void CloseSocketIfValid();
	void InvalidateSocket();


	//
	// Private data
private:
	shim::SOCKET mSocket = shim::kINVALID_SOCKET;
};

///////////////////////////////////////////////////////////////////////////////
}

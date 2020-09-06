#pragma once
#include "project.h"

#include "core_platform/win_shim.h"

#include "rftl/atomic"
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
	TCPSocket( TCPSocket&& rhs ); // WARNING: Not thread-safe
	TCPSocket& operator=( TCPSocket&& rhs ); // WARNING: Not thread-safe
	~TCPSocket();

	bool IsValid() const;
	bool IsListener() const;

	// NOTE: An empty hostname will result in attempting connections to local
	//  addresses, as opposed to "localhost" for loopback addresses
	static TCPSocket ConnectClientSocket( rftl::string hostname, uint16_t port );
	static TCPSocket BindServerSocket( bool preferIPv6, bool loopback, uint16_t port );
	static TCPSocket WaitForNewClientConnection( TCPSocket& listeningServerSocket );

	void Shutdown();


	//
	// Private methods
private:
	bool MakeNonBlocking();

	bool IsValidSocketHandle() const;
	shim::SOCKET GetMutableSocketHandle();
	void InitSocketHandle( shim::SOCKET socket );
	void ClearSocketHandle();

	void ShutdownSocketIfValid();
	void CloseSocketIfValid();


	//
	// Private data
private:
	rftl::atomic<bool> mShutdown = false;
	bool mListener = false;
	rftl::atomic<shim::SOCKET> mSocket = shim::kINVALID_SOCKET;
	static_assert( rftl::atomic<shim::SOCKET>::is_always_lock_free );
};

///////////////////////////////////////////////////////////////////////////////
}

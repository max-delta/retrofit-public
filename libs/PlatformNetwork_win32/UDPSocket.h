#pragma once
#include "project.h"

#include "core_platform/shim/win_shim.h"
#include "core_platform/IPAddressFwd.h"

#include "rftl/atomic"
#include "rftl/string"
#include "rftl/vector"


// Forwards
namespace win32 {
struct sockaddr;
}

namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMNETWORK_API UDPSocket final
{
	RF_NO_COPY( UDPSocket );

	//
	// Types and constants
public:
	using Buffer = rftl::vector<uint8_t>;

	//
	// Public methods
public:
	UDPSocket() = default;
	UDPSocket( UDPSocket&& rhs ); // WARNING: Not thread-safe
	UDPSocket& operator=( UDPSocket&& rhs ); // WARNING: Not thread-safe
	~UDPSocket();

	bool IsValid() const;

	// NOTE: Bound sockets can send to any port, but must recieve on their own
	static UDPSocket BindSocket( bool preferIPv6, bool loopback, uint16_t port );

	// Checks if an operation would be non-blocking
	// WARNING: Result may be invalidated by a related socket operation on
	//  another thread
	bool PeekSendNonBlocking();
	bool PeekReceiveNonBlocking();

	// NOTE: Intention is for buffer to be cleared and re-used by caller
	bool SendBuffer( Buffer const& buffer, IPv4Address const& address, uint16_t port );
	bool SendBuffer( Buffer const& buffer, IPv6Address const& address, uint16_t port );

	// NOTE: Empty buffer indicates connection failure
	// NOTE: Intention is for buffer to be re-used by caller
	bool ReceiveBuffer( Buffer& buffer, size_t maxLen );

	void Shutdown();


	//
	// Private methods
private:
	bool MakeNonBlocking();

	bool IsValidSocketHandle() const;
	shim::SOCKET GetMutableSocketHandle();
	void InitSocketHandle( shim::SOCKET socket );
	void ClearSocketHandle();

	bool PeekWriteNonBlocking();
	bool PeekReadNonBlocking();

	bool SendBuffer( Buffer const& buffer, win32::sockaddr const& destination, size_t destinationLen, uint16_t port );

	void ShutdownSocketIfValid();
	void CloseSocketIfValid();


	//
	// Private data
private:
	rftl::atomic<bool> mShutdown = false;
	rftl::atomic<shim::SOCKET> mSocket = shim::kINVALID_SOCKET;
	static_assert( rftl::atomic<shim::SOCKET>::is_always_lock_free );
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "core_platform/shim/win_shim.h"

#include "rftl/atomic"
#include "rftl/string"
#include "rftl/vector"


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMNETWORK_API TCPSocket final
{
	RF_NO_COPY( TCPSocket );

	//
	// Types and constants
public:
	using Buffer = rftl::vector<uint8_t>;

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

	// Checks if an operation would be non-blocking
	// WARNING: Result may be invalidated by a related socket operation on
	//  another thread
	bool PeekAcceptNewClientNonBlocking();
	bool PeekSendNonBlocking();
	bool PeekReceiveNonBlocking();

	// NOTE: Intention is for buffer to be cleared and re-used by caller
	bool SendBuffer( Buffer const& buffer );

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

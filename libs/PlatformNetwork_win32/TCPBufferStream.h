#pragma once
#include "project.h"

#include "core_platform/BufferStream.h"

#include "core/ptr/shared_ptr.h"

#include "rftl/shared_mutex"


// Forwards
namespace RF::platform::network {
class TCPSocket;
}

namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMNETWORK_API TCPIncomingBufferStream final : public IncomingBufferStream
{
	RF_NO_COPY( TCPIncomingBufferStream );

	//
	// Types and constants
private:
	static constexpr size_t kDefaultPrefetchSize = 4096;
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Public methods
public:
	TCPIncomingBufferStream( SharedPtr<TCPSocket> connectedSocket );
	TCPIncomingBufferStream( SharedPtr<TCPSocket> connectedSocket, size_t maxPrefetchSize );
	virtual ~TCPIncomingBufferStream() override = default;

	virtual size_t PeekNextBufferSize() const override;
	virtual Buffer FetchNextBuffer() override;
	virtual void Terminate() override;
	virtual bool IsTerminated() const override;

	SharedPtr<TCPSocket const> GetSocket() const;


	//
	// Private methods
private:
	void Prefetch( WriterLock const& lock ) const;
	void Hardfetch( WriterLock const& lock, size_t maxSize );


	//
	// Private data
private:
	SharedPtr<TCPSocket> const mSocket = nullptr;
	size_t const mMaxPrefetchSize = 0;

	mutable ReaderWriterMutex mPrefetchMutex;
	Buffer mPrefetchBuffer;
};

///////////////////////////////////////////////////////////////////////////////

class PLATFORMNETWORK_API TCPOutgoingBufferStream final : public OutgoingBufferStream
{
	RF_NO_COPY( TCPOutgoingBufferStream );

	//
	// Public methods
public:
	TCPOutgoingBufferStream( SharedPtr<TCPSocket> connectedSocket );
	virtual ~TCPOutgoingBufferStream() override = default;

	virtual bool StoreNextBuffer( Buffer&& buffer ) override;
	virtual void Terminate() override;
	virtual bool IsTerminated() const override;

	SharedPtr<TCPSocket const> GetSocket() const;


	//
	// Private data
private:
	SharedPtr<TCPSocket> const mSocket = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}

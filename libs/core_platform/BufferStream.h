#pragma once
#include "rftl/vector"


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Implementations are expected to be thread-safe
class IncomingBufferStream
{
	//
	// Types and constants
public:
	using Buffer = rftl::vector<uint8_t>;

	//
	// Public methods
public:
	virtual ~IncomingBufferStream() = default;

	// Returns the size of the next buffer
	// NOTE: Size may increase between this call and an attempt to fetch
	// NOTE: Size will never decrease between this call and an attempt to fetch
	// NOTE: Size does not necessarily represent the total size of all buffers
	// WARNING: Results are invalidated by fetches on other threads
	virtual size_t PeekNextBufferSize() const = 0;

	// Destructively read the next buffer
	// NOTE: If the peeked size was zero, a fetch may be blocking
	// NOTE: If the peeked size was non-zero, a fetch will not be blocking
	// NOTE: If the fetched size is zero, this indicates an error has occurred
	virtual Buffer FetchNextBuffer() = 0;

	// Force an unrecoverable termination of the source, releasing any blocked
	//  calls, and preventing future calls from blocking
	virtual void Terminate() = 0;

	// When true, the stream's source has been unrecoverably terminated, and no
	//  additional buffers will become available once peek returns zero
	// NOTE: One or more buffers may still be available after termination
	virtual bool IsTerminated() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

// NOTE: Implementations are expected to be thread-safe
class OutgoingBufferStream
{
	//
	// Types and constants
public:
	using Buffer = rftl::vector<uint8_t>;

	//
	// Public methods
public:
	virtual ~OutgoingBufferStream() = default;

	// Attempts to store a buffer
	// NOTE: Buffer unmoved/unmodified if a failure occurs
	virtual bool StoreNextBuffer( Buffer&& buffer ) = 0;

	// Force an unrecoverable termination of the destination, releasing any
	//  blocked calls, and preventing future calls from blocking
	virtual void Terminate() = 0;

	// When true, the stream's destination has been unrecoverably terminated,
	//  and no additional buffers will be accepted for storing
	// NOTE: Buffers already stored may or may not be discarded
	virtual bool IsTerminated() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

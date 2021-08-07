#pragma once
#include "core_platform/BufferStream.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/shared_mutex"


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

// WARNING: Multi-threaded operations on the underlying stream will likely
//  result in highly corrupt data in buffers, due to competing interleaved
//  fetching of sub-buffers. Avoid calls on the underlying stream to prevent
//  such an issue.
// NOTE: Multi-threaed operations on the stitcher will block heavily, to
//  prevent stitches from competing for data and interleaving
class IncomingBufferStitcher : public IncomingBufferStream
{
	RF_NO_COPY( IncomingBufferStitcher );

	//
	// Types and constants
private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Public methods
public:
	IncomingBufferStitcher( UniquePtr<IncomingBufferStream>&& underlyingStream );
	virtual ~IncomingBufferStitcher() override = default;

	virtual size_t PeekNextBufferSize() const override;
	virtual Buffer FetchNextBuffer() override;
	void Terminate() override;
	virtual bool IsTerminated() const override;

	// Will behave identically to the normal fetch, except it will not discard
	//  the buffer, and instead produce a clone
	// NOTE: If the clone size is non-zero, a subsequent normal fetch will
	//  return an identical buffer to the clone (there is no performance
	//  penalty for the subsequent fetch, and its buffer can be discarded)
	// NOTE: If the clone size is non-zero, a subsequent exact fetch of the
	//  same size or smaller will return bytes that match the clone (there will
	//  be a performance penalty for the partial buffer discard)
	Buffer CloneNextBuffer();

	// Attempts to stitch a single buffer, non-blocking. Returns the size of
	//  additional buffer.
	// NOTE: If the returned size is zero, this indicates either an error has
	//  occurred, or the stitch would've been blocking
	size_t TryStitchNextBuffer();

	// Will repeatedly fetch, append, and trim to ensure the buffer size is
	//  exactly the size requested
	// NOTE: If the fetched size is zero, this indicates an error has occurred
	// NOTE: Internal buffer is not lost on error, and may be accessed via a
	//  normal peek or fetch if any data remains
	Buffer FetchNextBuffer( size_t exactSize );


	//
	// Private data
private:
	UniquePtr<IncomingBufferStream> const mUnderlyingStream;

	mutable ReaderWriterMutex mWorkingBufferMutex;
	Buffer mWorkingBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}

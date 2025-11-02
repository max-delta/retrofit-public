#pragma once
#include "core/ptr/unique_ptr.h"

#include "rftl/iosfwd"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

class SeekHandle
{
	RF_NO_COPY( SeekHandle );
	RF_NO_MOVE( SeekHandle );

	//
	// Public methods
public:
	SeekHandle() = delete;
	explicit SeekHandle( UniquePtr<rftl::streambuf>&& streamBuf );
	~SeekHandle();

	size_t GetSeekableSize() const;

	// NOTE: Similar to rewind(...),
	void Rewind();

	// NOTE: Similar to fgetc(...),
	int GetChar();

	// NOTE: Similar to fread(...),
	size_t ReadBytes( void* buffer, size_t numBytes );

	// NOTE: Similar to fwrite(...),
	size_t WriteBytes( void const* buffer, size_t numBytes );

	// WARNING: This is unsafe to make streams around, as the stream will hold
	//  a pointer to the underlying buffer control object, but not have a
	//  mechanism to handle it being destroyed out from underneath the stream,
	//  so any created streams must be ensured to not outlive the file handle
	WeakPtr<rftl::streambuf> GetUnsafeTransientStreamBuffer();


	//
	// Protected data
protected:
	UniquePtr<rftl::streambuf> mStreamBuf;
};

///////////////////////////////////////////////////////////////////////////////
}

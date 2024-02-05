#pragma once
#include "project.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/iosfwd"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API FileHandle
{
	RF_NO_COPY( FileHandle );
	RF_NO_MOVE( FileHandle );

	//
	// Public methods
public:
	FileHandle() = delete;
	explicit FileHandle( UniquePtr<rftl::streambuf>&& streamBuf );
	~FileHandle();

	size_t GetFileSize() const;

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
	// Private data
private:
	UniquePtr<rftl::streambuf> mStreamBuf;
};

///////////////////////////////////////////////////////////////////////////////
}
